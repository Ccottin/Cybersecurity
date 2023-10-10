#!/usr/bin/env node
import fetch from "node-fetch";
import fs from 'fs';
import cheerio from 'cheerio';

class	webScrapper{
	flag = "";
	lenL = -1;
	path = undefined;
	url = undefined;
	validFlags = "rlp";

	img_urls = [];
	explored_pages = [];
	domain_url = undefined;
	
	constructor(args) {
//		console.log(this.flag, this.lenL, this.url);
		args.forEach((arg, index) => this.checkInput(arg, args[index + 1]));
		if (this.flag.indexOf('l') !== -1 && (this.flag.indexOf('r') === -1
			|| this.lenL === 0))
			throw new Error("please use -l with -r and specify a size");
		if (this.flag.indexOf('p') !== -1 && this.path === undefined)
			throw new Error("please use -p with a path");
		if (this.url === undefined)
			throw new Error("please provide an Url");
		if (this.path == undefined)
			this.path = "./data/";
		if (this.lenL <= 0)
			this.lenL = 5;
		console.log("parsed = ", this.flag, this.lenL, this.url, this.path);

		// verifier les droits du fichiers mais la nsm on trace avec fs.access()
	//	fs.mkdir(this.path);
	}

// trouver une solution pour le double check des arguments des flags

	checkInput(arg, nextarg) {
		if (arg.charAt(0) === '-')
		{
			for (let i = 1; i < arg.length; i++)
			{
				if (this.validFlags.indexOf(arg[i]) === -1) {		//check validite du flag
					throwErr("Invalid Flag Option");
				}
				if (this.flag.indexOf(arg[i]) === -1)				//ajout aux flags si il n est pas present
					this.flag += arg[i];
				if (arg[i] === 'l' && nextarg != undefined
					&& !isNaN(Number(nextarg))) {						//verifier que -l est join a une taille
					if (this.lenL !== 0)
						throw new Error("Please use a single l flag");
					this.lenL = Number(nextarg);
					if (this.lenL <= 0)
						throw new Error("-l size must be over 0");
				}
				else if (arg[i] === 'p' && nextarg != undefined)	//verifier que -p est join a un path
				{
					if (this.path !== undefined)
						throw new Error("Please use a single p flag");
					if (!(fs.statSync(nextarg).isDirectory()) )			//verifier le path est un dossier
						throw new Error("Please enter a valid path");
					this.path = nextarg;
				}
			}
		}
		else if (this.url === undefined)
			this.url = arg;
		else 
			throw new Error("Execute ./spider [-rlp] URL");
		}


//	let	get_srcs = parse_img_url(html_data);
//	get_img(get_srcs);

	set_domain_url() {
		let	index = this.url.indexOf("://") + 4;
		index = this.url.indexOf("/", index);
		if (index === -1)
			this.domain_url = this.url + "/";
		else
			this.domain_url = this.url.substring(0, index + 1);
	}

	access_lower_depth(html_data, depth_lvl) {
		if	(depth_lvl == 0)
			return ;

		console.log("depth reached = ", depth_lvl);
		let page = cheerio.load(html_data);
		let body = page('body');
		let	bodyElems = body.find('*');
		let	domain_url = this.domain_url;
		bodyElems.each(async (index, element) => {
			let href = page(element).attr('href');
			if (href && (href.indexOf("mailto:") != -1	//refuser les liens personnalises
				|| href.indexOf("tel:") != -1
				|| href.indexOf("file:") != -1
				|| href.indexOf("myapp:") != -1)) {
				return ;
			}
			if (href && href.indexOf('#') !== -1) {
				href = href.slice(0, href.indexOf('#'));
			}
			if (href && href.startsWith(this.domain_url)) {
			//	console.log(1 + href);
				
			}
			else if (href && href.startsWith("/")) {
				href = this.domain_url + href.slice(1);
			//	console.log(2 + href);
			}
			else if (href && href.indexOf("://") == -1) {
				href = this.domain_url + href;
			//	console.log(3 + href);
			}
			else
				return ;
			if (this.explored_pages.indexOf(href) != -1)
				 return ;
			this.explored_pages.push(href);

			let img_list = parse_img_url(html_data);		//get urls and extract images
			for (let i = 0 ; i < img_list.length; i++)
			{
				console.log("path = ", this.path);
				if (this.img_urls.indexOf(img_list[i]) == -1) {
					await get_img(this.path, img_list[i],
						get_img_name(img_list[i]));
					this.img_urls.push(img_list[i]);
				}
			}
			let new_html_data = await get_html_data(href);
			if (new_html_data == undefined)
				return ;
			this.access_lower_depth(new_html_data, depth_lvl - 1);
		});
	}
}

async function	get_html_data(url) {
	let	html_data;

	try {
		await fetch(url)
			.then(response => response.text())
			.then(html => { html_data = html; })
	}
	catch (error) {
		return ;
	}
	return (html_data);
}

function	parse_img_url(html_data){

	let		ret = [];
	let		nb = 0;
	let		i;
	let		y;

	i = html_data.indexOf("<img ");
	while (i !== -1)
	{	
		i = html_data.indexOf("src", i);
		i = html_data.indexOf("\"", i);
		y = html_data.indexOf("\"", i + 1);
		ret[nb++] = html_data.substring(i + 1, y);
		i = html_data.indexOf("<img ", y);
	}
	console.log("ret = ", ret);
	return ret;	
}

//a faire -> modifier, chopper le nom de l image, commencer le recuersif

function	get_img_name(src)
{
	return (src.substring(src.lastIndexOf("/"),
		src.lenght));
}

async function	get_img(path, src, imgName)
{
	await fetch(src)
	.then(response => {
    if (response.ok) {
		const fileOut = fs.createWriteStream(path + imgName);
		response.body.pipe(fileOut);		// Pipe la réponse HTTP dans le fichier
		fileOut.on('finish', () => {
			console.log(`L'image a été téléchargée avec succès`);
		});
	}})
	.catch(error => {
		console.error(src);
		console.error('Downloadling problem: ', error);
  });
}

//MainProcess
async function	main(args) {
	args.splice(0, 2);

	let wS;
	let	html_data;

	try {
		wS = new webScrapper(args);
		await fetch(wS.url)
			.then(response => response.text())
			.then(html => { html_data = html; })
	}

	catch (error) {
		console.log(error);
		console.log(error.message);
	}
	if (wS !== undefined) {
		wS.set_domain_url();
		await wS.access_lower_depth(html_data, wS.lenL);
	//	parse_img_url(html_data);
	}

}

main(process.argv);
