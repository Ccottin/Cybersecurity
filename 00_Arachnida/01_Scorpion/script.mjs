#!/usr/bin/env node

import fs from 'fs';
import sharp from 'sharp';
import ExifParser from 'exif-parser';

function	access_exif_data(file) {
	fs.readFile(file, (err, data) => {
		if (err)
			console.error(err);
		else {
			const parser = ExifParser.create(data);
			const exifData = parser.parse();
			console.log(exifData);
			console.log("==================");
			console.log( "EXIF informations :"
				,"\nExif exifData starts at", exifData.app1Offset, "bytes"
				,"\nResolution Unit: ", exifData.tags.ResolutionUnit
				,"\nXResolution(Dot per Unit): ", exifData.tags.XResolution
				,"\nYResolution(Dot per Unit): ", exifData.tags.YResolution
				,"\nArtist: ", exifData.tags.Artist
				,"\nCopyright: ", exifData.tags.Copyright);

			}
	});
}

async function access_metadatas(file) {
	const data = await sharp(file).metadata();
	console.log(file.substring(file.lastIndexOf("/") + 1) + "'s metadatas : ");
	console.log("==================");
	console.log(data);
	,"\nFormat: ", data.format
	,"\nWidth: ", data.width + "px"
	,"\nHeight: ", data.height + "px"
	,"\nColors display: ", data.space
	,"\nColors channels: ", data.channels
	,"   (3 channels with sRGB usually stands for Red Green and Blue)"
	,"\ntype per channels: ", data.depth
	,"\nResolution Unit: ", data.resolutionUnit
	,"\nDots Per Inch (DPI): ", data.density
	,"\nChroma Subsampling: ", data.chromaSubsampling);
	data.isProgressive ? console.log("The image will appear progressively") :
		console.log("The image will be loaded lines per lines");
	data.hasProfile ? console.log("The image has a color profile") :
		console.log("The image has no color profile");
	data.hasAlpha ? console.log("The image got a transparent channel") :
		console.log("The image got no transparent channel");

	if (data.format == 'jpeg')
		access_exif_data(file);
}

function	main(args) {
	args.splice(0,2);
	for (let i = 0; i < args.length; i++) {
		access_metadatas(args[i]);
	}
}

main(process.argv);
