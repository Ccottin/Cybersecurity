#!/usr/bin/env node
import fetch from "node-fetch";

class	webScrapper{
	flag = "";
	lenL = 0;
	url = undefined;
	validFlags = "rlp";
	
	constructor(args) {
		console.log(this.flag, this.lenL, this.url);
		args.forEach((arg) => this.checkInput(arg));
		console.log(this.flag, this.lenL, this.url);
		if (this.flag.indexOf('l') !== -1 && (this.flag.indexOf('r') === -1
			|| this.lenL === 0))
			throw new Error("please use -l with -r and specify a size");
		if (this.url === undefined)
			throw new Error("please provide an Url");

	}

// gerer les -l et les -p par pairs.

	checkInput(arg) {
		if (arg.charAt(0) === '-')
		{
			for (var i = 1; i < arg.length; i++)
			{
				if (this.validFlags.indexOf(arg[i]) === -1) {
					throwErr("invalid Flag Option");
				}
				if (this.flag.indexOf(arg[i]) === -1)
					this.flag += arg[i];
			}
		}
		else if (this.flag !== "" && this.flag.indexOf('l') > 0 && !isNaN(Number(arg))) {
			if (this.lenL !== 0)
			{
				console.log("bp1", this.flag, this.lenL, this.url, Number(arg));
				throw new Error("too many sizes for -l flag");
			}
			console.log("bp2", this.flag, this.lenL, this.url, Number(arg));
			this.lenL = Number(arg);
		}
		else if (this.url === undefined)
			this.url = arg;
		else
			throw new Error("execute ./spider [-rlp] URL");

		}
}


//MainProcess

const args = process.argv;
args.splice(0, 2);

var wS;
try {
	wS = new webScrapper(args);
	fetch(wS.url)
	//	.then(response => response.text())
//	.then(data => console.log(data))

}

catch (error) {
	console.log(error);
	console.log(error.message);
}

