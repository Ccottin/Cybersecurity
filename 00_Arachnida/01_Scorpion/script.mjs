#!/usr/bin/env node

import fs from 'fs';
import sharp from 'sharp';
import ExifParser from 'exif-parser';
import exifr from 'exifr';


function access_icc_data(input) {
	exifr.parse(input, {tiff: false, icc: true})
		.then(output => {
		console.log('----ICC');
		console.log(output);
	})
}

function	access_exif_data(file) {
	fs.readFile(file, (err, data) => {
		if (err)
			console.error(err);
		else {
			const parser = ExifParser.create(data);
			const exifData = parser.parse();
			console.log( "----EXIF informations : ");
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
	console.log("==================");
	console.log(file.substring(file.lastIndexOf("/") + 1) + "'s metadatas : ");
	console.log(data);

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

	if (data.format == 'jpeg')
		access_exif_data(file);
//	if (data.icc)
//		access_icc_data(data.icc);
	

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
		exifr.parse(args[i])
			.then(output => console.log(output));
		access_metadatas(args[i]);
	}
}

main(process.argv);
