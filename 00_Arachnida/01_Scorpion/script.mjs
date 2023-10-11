#!/usr/bin/env node

import fs from 'fs';
import ExifParser from 'exif-parser';

function	access_exif_data(file) {
	fs.readFile(file, (err, data) => {
		if (err)
			console.error(err);
		else {
			const parser = ExifParser.create(data);
			const exifData = parser.parse();
			console.log(exifData);
		}
	})
}

function	main(args) {
	args.splice(0,2);
	for (let i = 0; i < args.length; i++)
		access_exif_data(args[i]);
}

main(process.argv);
