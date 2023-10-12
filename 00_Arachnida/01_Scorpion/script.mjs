#!/usr/bin/env node

import fs from 'fs';
import sharp from 'sharp';
import ExifParser from 'exif-parser';
import exifr from 'exifr';

async function	access_jfif_data(input) {
	let output; 
	try {
		output = await exifr.parse(input, {tiff: false,
			jfif: true, ifd0: false, exif: false, gps: false})
	} catch (err) {
		console.error(err.name, "Cannot access JFIF datas");
		return ;
	}
	console.log('----JFIF');
	console.log(output);
}
async function	access_IHDR(input){
	let output; 
	try {
		output = await exifr.parse(input, {tiff: false,
			ihdr: true,  ifd0: false, exif: false, gps: false })
	} catch (err) {
		console.error(err.name, "Cannot access IHDR datas");
		return ;
	}
	console.log('----IHDR');
	console.log(output);
}

async function	access_icc_data(input) {
	let output; 
	try {
		output = await exifr.parse(input, {tiff: false,
			icc: true, ifd0: false, exif: false, gps: false })
	} catch (err) {
		console.error(err.name, "Cannot access ICC datas");
		return ;
	}
	console.log('----ICC');
	console.log(output);
}

async function	access_exif_data(file) {
	fs.readFile(file, (err, data) => {
		if (err)
			console.error(err);
		else {
			const parser = ExifParser.create(data);
			const exifData = parser.parse();
			console.log( "----EXIF");
			console.log(exifData);
		}
	});
}

async function access_metadatas(file) {
	let data;
	try {
		data = await sharp(file).metadata();
	} catch (err) {
		console.log("unvalid image file");
		return ;
	}

	const stats = fs.statSync(file);
	if (data.exif !== undefined)
		delete data.exif;
	if (data.icc !== undefined)
		delete data.icc;
	
	console.log("==================");
	console.log(file.substring(file.lastIndexOf("/") + 1) + "'s metadatas :\n");
	console.log("Creation date", stats.ctime.toString().substring(11));

	console.log("Basics infos:\n", data);
	if (data.isProgressive != undefined) {
		data.isProgressive ? console.log("The image will appear progressively") :
			console.log("The image will be loaded lines per lines");
	}
	if (data.hasProfile != undefined) {
		data.hasProfile ? console.log("The image has a color profile") :
			console.log("The image has no color profile");
	}
	if (data.hasAlpha != undefined) {
		data.hasAlpha ? console.log("The image got a transparent channel") :
			console.log("The image got no transparent channel");
	}

	if (data.format == 'jpeg') {
		await access_exif_data(file);
		await access_jfif_data(file);
	}
	if (data.icc)
		await access_icc_data(file);
	if (data.format == 'png')
		await access_IHDR(file);

	console.log("==================");
}

async function	main(args) {
	args.splice(0,2);
	for (let i = 0; i < args.length; i++) {
		let extension = args[i].substring(args[i].lastIndexOf('.'));
		if (extension !== ".jpg" && extension !== ".jpeg"
			&& extension !== ".png" && extension !== ".gif" 
			&& extension !== ".jpg")
			continue ;
		await access_metadatas(args[i]);
		await new Promise(r => setTimeout(r, 50));
	}
}

main(process.argv);
