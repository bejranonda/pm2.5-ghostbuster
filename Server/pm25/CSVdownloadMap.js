const https = require('https');
const fs = require('fs');

// URL to download the CSV file
const url = 'https://firms.modaps.eosdis.nasa.gov/api/area/csv/36e83b24f187cfcde5b0b4d1cdcd1330/VIIRS_SNPP_NRT/96,5,105,20/1'; // only Thailand
//const url = 'https://firms.modaps.eosdis.nasa.gov/api/area/csv/36e83b24f187cfcde5b0b4d1cdcd1330/VIIRS_SNPP_NRT/80,-15,120,30/1'; // around Thailand
//const url = 'https://firms.modaps.eosdis.nasa.gov/api/area/csv/36e83b24f187cfcde5b0b4d1cdcd1330/VIIRS_SNPP_NRT/world/1/'; // World
//5.622578509277092, 96.0039365735059
//20.700658286787487, 105.58401404668207
// New filename for the extracted "lat" and "long" data
const newFilename = '../../var/www/html/csv/fire.csv';

// Function to extract the "lat" and "long" columns from the CSV data
function extractCoordinates(data) {
  const lines = data.trim().split('\n');
  const headers = lines.shift().split(',');
  const latIndex = headers.indexOf('latitude');
  const longIndex = headers.indexOf('longitude');
    const brightnessIndex = headers.indexOf('bright_ti4');
  const coordinates = lines.map(line => {
    const values = line.split(',');
    return [values[latIndex], values[longIndex], values[brightnessIndex]];
  });
  //return { headers: ['lat', 'long'], data: coordinates };
  return { headers: ['latitude', 'longitude', 'brightness'], data: coordinates };
}

// Function to download the CSV file from the URL
function downloadCSV() {
  https.get(url, res => {
    let data = '';
    res.on('data', chunk => {
      data += chunk;
    });
    res.on('end', () => {
      const { headers, data: coordinates } = extractCoordinates(data);
      const csv = [headers.join(',')].concat(coordinates.map(c => c.join(','))).join('\n');
      fs.writeFile(newFilename, csv, err => {
        if (err) {
          console.error(err);
        } else {
          console.log(`Saved ${newFilename}`);
        }
      });
    });
  }).on('error', err => {
    console.error(err);
  });
}

// Download the CSV file every 15 minutes
setInterval(downloadCSV, 15 * 60 * 1000);
//setInterval(downloadCSV, 10 * 1000);
