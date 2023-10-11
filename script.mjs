#!/usr/bin/env node
import fetch from "node-fetch";
import fs from 'fs';
import cheerio from 'cheerio';


/*****************CLASS WEBSCRAPPER*******************/

class	webScrapper {
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
		for (let i = 0; i < args.length; i += this.checkInput(args[i], args[i + 1], i))
			{ }
		if (this.flag.indexOf('l') !== -1 && (this.flag.indexOf('r') === -1
			|| this.lenL === 0))
			throw new Error("please use -l with -r and specify a size");
		if (this.flag.indexOf('p') !== -1 && this.path === undefined)
			throw new Error("please use -p with a path");
		if (this.url === undefined)
			throw new Error("please provide an Url");
		if (this.path == undefined)
			this.path = "./data/";
		this.checkPath();
		if (this.lenL <= 0 && this.flag.indexOf('r') != -1)
			this.lenL = 1;
		this.set_domain_url();
	}


	/*****************CHECKER/PARSER*******************/
	
	async checkPath() {
		try {
			await fs.access(this.path, fs.constants.W_OK, (err) => {
				if (err) {
					console.error("Please ensure you have proper right for your path");
					throw (err);
				}
			});												//verifier les droits
			fs.statSync(this.path).isDirectory();			//verifier le path est un dossier
		} catch (error) {
			console.error("Please check your file path");
			throw (error);
		}
	}

	checkInput(arg, nextarg, index) {
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
					if (this.lenL !== -1)
						throw new Error("Please use a single l flag");
					this.lenL = Number(nextarg);
					if (this.lenL <= 0)
						throw new Error("-l size must be over 0");
					return (2);
				}
				else if (arg[i] === 'p' && nextarg != undefined)	//verifier que -p est join a un path
				{
					if (this.path !== undefined)
						throw new Error("Please use a single p flag");
					this.path = nextarg;
					return (2);
				}
			}
		}
		else if (this.url === undefined)
			this.url = arg;
		else 
			throw new Error("Execute ./spider [-rlp] URL");
		return (1);
	}

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
	filter_domains_links(html_data) {
		let $ = cheerio.load(html_data);
		let $body = $('body [href]');
		let hrefs = [];
		let	i = 0;
		$body.each((index, element) => {
			let		current = $(element).attr('href');
			if (is_personals_url(current))						//refuser les liens personnalises
				return ;
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
		if	(depth_lvl <= 0)
			return ;

		let hrefs = this.filter_domains_links(html_data, this.domain_url);
		hrefs.forEach(async (element) => {

			this.explored_pages.push(element);

			let img_list = this.parse_img_url(html_data);
			for (let i = 0 ; i < img_list.length; i++)
			{
				if (this.img_urls.indexOf(img_list[i]) == -1) {
					this.img_urls.push(img_list[i]);
					await get_img(this.path, img_list[i],
						get_img_name(img_list[i]), i);
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

function	is_personals_url(current) {
	if (!current || current.indexOf("mailto:") != -1	
		|| current.indexOf("tel:") != -1
		|| current.indexOf("file:") != -1
		|| current.indexOf("myapp:") != -1)
		return (true);
	return (false);
}

async function	get_html_data(url) {
	let	html_data;

	try {
		await fetch(url)
			.then(response => response.text())
			.then(html => { html_data = html; })
	}
	catch (error) {
		console.error(error);
		return (undefined);
	}
	return (html_data);
}

function	get_img_name(src)
{
	let name = src.substring(src.lastIndexOf("/") + 1, src.lenght);
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
				console.log(path, index, imgName);
				const fileOut = fs.createWriteStream(path + index + imgName);
				response.body.pipe(fileOut);		// Pipe la réponse HTTP dans le fichier
			/*	fileOut.on('finish', () => {
					console.log(imgName + index);
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
		console.error(error);
		return ;
	}
	if (wS !== undefined && html_data != undefined) {
		await wS.access_lower_depth(html_data, wS.lenL);
		wS.parse_img_url(html_data);
	}

}

main(process.argv);
