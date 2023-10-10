#!/usr/bin/env node
import fetch from "node-fetch";
import fs from 'fs';
import cheerio from 'cheerio';


/*****************CLASS WEBSCRAPPER*******************/

class	webScrapper{
	flag = "";
	lenL = -1;
	protocol = undefined;
	path = undefined;
	url = undefined;
	validFlags = "rlp";

	img_urls = [];
	explored_pages = [];
	domain_url = undefined;
	
	/*****************CONSTRUCTOR*******************/
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

	/*****************PARSER*******************/
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

	/*****************SETTER*******************/
	set_domain_url() {
		let	index = this.url.indexOf("://") + 4;
		this.protocol = this.url.slice(0, index - 1);
		index = this.url.indexOf("/", index);
		if (index === -1)
			this.domain_url = this.url + "/";
		else
			this.domain_url = this.url.substring(0, index + 1);
	}


	/*****************METHODS*******************/
	filter_domains_links(html_data) {					//c'est vraiment pas bo
		let $ = cheerio.load(html_data);
		let $body = $('body [href]');
		let hrefs = [];
		let	i = 0;
		$body.each((index, element) => {
			let		current = $(element).attr('href');
			if (!current || current.indexOf("mailto:") != -1	//refuser les liens personnalises
				|| current.indexOf("tel:") != -1
				|| current.indexOf("file:") != -1
				|| current.indexOf("myapp:") != -1) {
				return ;
			}
			switch (current.indexOf('#')) {						//trimer les references a des 
				case -1:										//sections
					break ;
				case 0:
					return ;
				default :
					current = current.slice(0, current.indexOf('#'));
					break ;
			}
			if (current.startsWith("/")) 
				current = this.domain_url + current.slice(1);
			 else if (current.indexOf("://") == -1) 
				current = this.domain_url + current;
			 else if (!current.startsWith(this.domain_url))
				return ;
			
			if (this.explored_pages.indexOf(current) != -1
				|| hrefs.indexOf(current) != -1)
				return ;
			hrefs[i++] = current;
		});
		return (hrefs);
	}

	parse_img_url(html_data){

	let		ret = [];
	let		nb = 0;
	let		i;
	let		y;
	let		current;

	i = html_data.indexOf("<img ");
	while (i !== -1)
	{	
		i = html_data.indexOf("src", i);
		i = html_data.indexOf("\"", i);
		y = html_data.indexOf("\"", i + 1);
		current = html_data.substring(i + 1, y);
		if (this.img_urls.indexOf(current) == -1
			&& ret.indexOf(current) == -1)
			ret[nb++] = format_img_url(current, this.domain_url, this.protocol);
		i = html_data.indexOf("<img ", y);
	}
	return ret;	
}

	access_lower_depth(html_data, depth_lvl) {
		if	(depth_lvl == 0)
			return ;

	//	console.log("depth reached = ", depth_lvl);
		let hrefs = this.filter_domains_links(html_data, this.domain_url);
		hrefs.forEach(async (element) => {

			this.explored_pages.push(element);

			let img_list = this.parse_img_url(html_data);
	//		console.log(img_list);//get urls and extract images
			for (let i = 0 ; i < img_list.length; i++)
			{
				if (this.img_urls.indexOf(img_list[i]) == -1) {
					await get_img(this.path, img_list[i],
						get_img_name(img_list[i]), i);
					this.img_urls.push(img_list[i]);
				}
			}
			let new_html_data = await get_html_data(element);
			if (new_html_data == undefined)
				return ;
			this.access_lower_depth(new_html_data, depth_lvl - 1);
		});
	}
}

/*****************EXTERNS FUNCTIONS*******************/


async function	get_html_data(url) {
	let	html_data;

	try {
		await fetch(url)
			.then(response => response.text())
			.then(html => { html_data = html; })
	}
	catch (error) {
		console.log(error);
		return (undefined);
	}
	return (html_data);
}

function	get_img_name(src)
{
	let name = src.substring(src.lastIndexOf("/"), src.lenght);
	if (name.length > 200) {
		name = name.slice(0, 200);
	}
	return (name);
}

function	format_img_url(src, domain_url, protocol)
{
	if (src.startsWith(domain_url))
		return (src);
	if (src.startsWith("//"))
		return (protocol + src.slice(2));
	if (src.startsWith("/"))
		return (domain_url + src.slice(1));
	else
		return (domain_url + src);
}


async function	get_img(path, src, imgName, index)
{
	await fetch(src)
		.then(response => {
			if (response.ok) {
				const fileOut = fs.createWriteStream(path + imgName + index);
				response.body.pipe(fileOut);		// Pipe la réponse HTTP dans le fichier
			/*	fileOut.on('finish', () => {
					console.log(`L'image a été téléchargée avec succès`);
				});
		*/	}})
		.catch(error => {
			console.error(src);
			console.error('Downloadling problem: ', error);
		});
}

/*****************MAIN*******************/
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
	console.log(html_data);
	if (wS !== undefined && html_data != undefined) {
		wS.set_domain_url();
		await wS.access_lower_depth(html_data, wS.lenL);
		//	parse_img_url(html_data);
	}

}

main(process.argv);
