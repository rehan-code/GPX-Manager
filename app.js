'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let sharedLib = ffi.Library('./libgpxparser', {
  'filenametogpxJSON': [ 'string', ['string']],
  'filenametoRouteListJSON': [ 'string', ['string']],
  'filenametoTrackListJSON': [ 'string', ['string']],
  'createGPX': [ 'int', ['string', 'string']],
  'addRtetoFile': [ 'int', ['string', 'string', 'string']],
  'routeListJSONBwetweenPath': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  'trackListJSONBwetweenPath': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  'renameComp': [ 'int', ['string', 'string', 'string']],
  'details': [ 'string', ['string', 'string']],
  'filenametoRteWptListJSON': ['string', ['string', 'string']],
});


//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

//send array of files
app.get('/endpoint2', function(req , res){
  let string = '';
  let data = {};
  let path = require('path');
  let fs = require('fs');
  //joining path of directory 
  let directoryPath = path.join(__dirname, 'uploads');
  var files = fs.readdirSync(directoryPath);

  files.forEach(function (file) {
    if (file.substr(file.length-4, file.length-1)==='.gpx') {
      let fileData = sharedLib.filenametogpxJSON("uploads/"+file);
      //console.log('test: '+fileData);
      if (fileData != '{}') {
        string = string + '"' + file + '":' + fileData + ",";
      }
    }
  });
  // string[string.length-1] = "}";
  if (string.length != 0) {
    string = string.substr(0,string.length-1);
    string = '{' + string + '}';

    data = JSON.parse(string);
  }
  //console.log({'simple.gpx':{"version":1.1,"creator":"Denis Nikitenko", "numWaypoints":3, "numRoutes":1, "numTracks":1}});
  //console.log(data);
  res.send(data);
});

//send gpx file data based on drop down option press
app.get('/openfile', function(req , res){
  let rteStr = JSON.parse(sharedLib.filenametoRouteListJSON('uploads/'+req.query.filename));
  let trkStr = JSON.parse(sharedLib.filenametoTrackListJSON('uploads/'+req.query.filename));
  console.log(rteStr);
  console.log(trkStr);
  res.send(
    {
      routes: rteStr,
      tracks: trkStr
    }
  );
});

//create gpx file based on filename
app.get('/creategpx', function(req , res){
  let path = require('path');
  let directoryPath = path.join(__dirname, 'uploads');
  var files = fs.readdirSync(directoryPath);
  let filename = req.query.filename;
  let invalid = 0;
  let exists = 0;

  if (filename.substr(filename.length-4, filename.length-1)!=='.gpx') {
    invalid = 1;
  }
  files.forEach(function (file) {
    if (file == filename) {
      invalid = 1;
      exists = 1;
    }
  });

  if (invalid == 1) {
    if (exists == 1) {
      console.log("Error: Couldn't create file: "+filename+" already exists.");
      res.send({fail: 2});
    } else {
      console.log("Error: Couldn't create file: "+filename+" does not contain .gpx.");
      res.send({fail: 1});
    }
  } else {
    if (sharedLib.createGPX('uploads/'+filename, '{"version":1.1,"creator":"Rehan Nagoor"}')==1) {
      console.log("file: "+filename+" created successfully.");
      res.send({fail: 0});
    } else {
      console.log("Error: "+filename+" could not be created.");
      res.send({fail: 3});
    }
  }
});

//Add a route to a file
app.get('/addRoute', function(req , res){
  res.send({success: sharedLib.addRtetoFile('uploads/'+req.query.filename, JSON.stringify(req.query.routeName), req.query.wptlist)});
});

//send gpx file data based on path between
app.get('/findpath', function(req , res){
  let path = require('path');
  let fs = require('fs');
  //joining path of directory 
  let directoryPath = path.join(__dirname, 'uploads');
  var files = fs.readdirSync(directoryPath);

  let rteList = [];
  let trkList = [];
  
  files.forEach(function (file) {
    if (file.substr(file.length-4, file.length-1)==='.gpx') {
      rteList = rteList.concat(JSON.parse(sharedLib.routeListJSONBwetweenPath('uploads/'+file, req.query.spLat, req.query.spLon, req.query.epLat, req.query.epLon, req.query.accuracy)));
      trkList = trkList.concat(JSON.parse(sharedLib.trackListJSONBwetweenPath('uploads/'+file, req.query.spLat, req.query.spLon, req.query.epLat, req.query.epLon, req.query.accuracy)));
    }
  });

  res.send(
    {
      routes: rteList,
      tracks: trkList
      //tracks: [{"name": "MountSteele Trail","numPoints": 3 , "len": 340.0, "loop":false}, {"name": "some Trail","numPoints": 5 , "len": 240.0, "loop":false}]
    }
  );
});

app.get('/rename', function(req , res){
  let ret = sharedLib.renameComp(req.query.filename, req.query.componentName, req.query.newName);
  console.log("rename status: "+ ret);
  res.send(
    {
      success: ret
    }
  )
});

app.get('/details', function(req , res){
  let test = sharedLib.details(req.query.filename, req.query.componentName);
  let array = JSON.parse(test);

  let ret;
  if (array.length == 0) {
    ret = 0;
  } else {
    ret = 1;
  }
  res.send(
    {
      success: ret,
      list: array
    }
  )
});

app.get('/login', async function(req , res){
  let ret = 1;
  const mysql = require('mysql2/promise');
  let connection;
  let dbConf = req.query;
  //console.log(req.query);

  try{
    // create the connection
    connection = await mysql.createConnection(dbConf);
    
    const [rows1, fields1] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'FILE';");
    if (rows1[0]['COUNT(*)'] == 0) {
      await connection.execute("CREATE TABLE FILE (gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL, PRIMARY KEY(gpx_id));");
      console.log('FILE table created');
    }

    const [rows2, fields2] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'ROUTE';");
    if (rows2[0]['COUNT(*)'] == 0) {
      await connection.execute("CREATE TABLE ROUTE (route_id INT AUTO_INCREMENT, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE);");
      console.log('ROUTE table created');
    }

    const [rows3, fields3] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'POINT';");
    if (rows3[0]['COUNT(*)'] == 0) {
      await connection.execute("CREATE TABLE POINT (point_id INT AUTO_INCREMENT, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, PRIMARY KEY(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE);");
      console.log('POINT table created');
    }

    //console.log(rows1[0]['COUNT(*)']);
  }catch(e){
    console.log("Login Error: "+e);
    ret = 0;
  }finally {
    if (connection && connection.end) connection.end();
  }

  res.send(
    {
      success: ret
    }
  )
});

app.get('/display-DB-Status', async function(req , res){
  let retString = '';
  let dbConf = req.query.data;
  let files = 0;
  let routes = 0;
  let points = 0;
  //console.log(req.query);

  if (dbConf === '') {
    retString = 'Please Login first';
  } else {
    const mysql = require('mysql2/promise');
    let connection;
    try{
      // create the connection
      connection = await mysql.createConnection(dbConf);
      
      const [rows1, fields1] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'FILE';");
      if (rows1[0]['COUNT(*)'] == 0) {
        retString = 'Table FILE not created.\n';
      } else {
        const [rows11, fields11] = await connection.execute("SELECT COUNT(*) FROM FILE;");
        retString = 'Database has ' + rows11[0]['COUNT(*)'] + ' files, ';    
        files = rows11[0]['COUNT(*)'];
      }
  
      const [rows2, fields2] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'ROUTE';");
      if (rows2[0]['COUNT(*)'] == 0) {
        retString = retString + 'Table ROUTE not created.\n';
      } else {
        const [rows21, fields21] = await connection.execute("SELECT COUNT(*) FROM ROUTE;");
        retString = retString + rows21[0]['COUNT(*)'] + ' routes, and ';        
        routes = rows21[0]['COUNT(*)'];
      }
  
      const [rows3, fields3] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'POINT';");
      if (rows3[0]['COUNT(*)'] == 0) {
        retString = retString + 'Table POINT not created.\n';
      } else {
        const [rows31, fields31] = await connection.execute("SELECT COUNT(*) FROM POINT;");
        retString = retString + rows31[0]['COUNT(*)'] + ' points';        
        points = rows31[0]['COUNT(*)'];
      }
  
      console.log('Display DB status: success');
    }catch(e){
      console.log("Display Database Status Error: "+e);
      retString = retString + ' Connection Error occured\n Please re-enter login details and try again';
    }finally {
      if (connection && connection.end) connection.end();
    }
  }

  res.send(
    {
      string: retString,
      numFiles: files,
      numRoutes: routes,
      numPoints: points
    }
  )
});

app.get('/store-files', async function(req , res){
  let ret = await storeFilesToTables(req.query.data);
  //console.log(ret);
  res.send(
    {
      success: ret
    }
  )
});

app.get('/clear-sql-table', async function(req , res){
  let ret = await clearSQLTable(req.query.data);
  //console.log(ret);
  res.send(
    {
      success: ret
    }
  )
});

app.get('/update-sql-table', async function(req , res){
  let ret = await clearSQLTable(req.query.data);
  if (ret == 1) {
    ret = await storeFilesToTables(req.query.data);
  }
  //console.log(ret);
  res.send(
    {
      success: ret
    }
  )
});

async function storeFilesToTables(dbConf) {
  let ret = 1;
  let path = require('path');
  let fs = require('fs');
  //joining path of directory 
  let directoryPath = path.join(__dirname, 'uploads');
  //console.log(req.query);

  let mysql = require('mysql2/promise');
  let connection;
  try{
    // create the connection
    connection = await mysql.createConnection(dbConf);
    
    const [rows1, fields1] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'FILE';");
    if (rows1[0]['COUNT(*)'] == 0) {//table was not created
      console.log('Table FILE not created.');
    } else {
      var files = fs.readdirSync(directoryPath);

      for (let index3 = 0; index3 < files.length; index3++) {//for each file
        let file = files[index3];
        
        if (file.substr(file.length-4, file.length-1)==='.gpx') {
          let fileData = sharedLib.filenametogpxJSON("uploads/"+file);
          //console.log(fileData);
          if (fileData != '{}') {//if valid

            //adding files to FILE table
            let tempdata = JSON.parse(fileData);
            const [rows12, fields12] = await connection.execute("SELECT COUNT(*) FROM FILE WHERE file_name='"+file+"';");
            if (rows12[0]['COUNT(*)'] == 0) {//if file doesnt already exist
              //insert files to FILE table
              //console.log("INSERT INTO FILE VALUES (NULL, '"+file+"', "+tempdata.version+", '"+tempdata.creator+"');");
              await connection.execute("INSERT INTO FILE VALUES (NULL, '"+file+"', "+tempdata.version+", '"+tempdata.creator+"');");
              const [rows2, fields2] = await connection.execute("SELECT * FROM FILE WHERE file_name='"+file+"';");
              let gpxid = rows2[0]['gpx_id'];
              //console.log(gpxid);

              //adding routes to ROUTE table
              let tempdata2 = JSON.parse(sharedLib.filenametoRouteListJSON('uploads/'+file));
              if (tempdata2!==undefined && tempdata2.length!==0) {
                for (let index2 = 0; index2 < tempdata2.length; index2++) {//for every route in the file
                  let item = tempdata2[index2];
                  let rteName = 'NULL';
                  if (item.name !='') {
                    rteName = item.name;
                  }
                
                  const [rows3, fields3] = await connection.execute("SELECT COUNT(*) FROM ROUTE WHERE route_name='"+rteName+"' AND route_len="+item.len+" AND gpx_id="+gpxid+";");
                  if (rows3[0]['COUNT(*)'] == 0) {//if route doesnt already exist in ROUTE table
                    await connection.execute("INSERT INTO ROUTE VALUES (NULL, '"+rteName+"', "+item.len+", "+gpxid+");");
                    
                    const [rows31, fields31] = await connection.execute("SELECT * FROM ROUTE WHERE route_name='"+rteName+"' AND route_len="+item.len+" AND gpx_id="+gpxid+";");
                    let routeid = rows31[0]['route_id'];
                    //console.log(gpxid);

                    //adding waypoints to POINT table
                    let rtename = rows31[0]['route_name'];
                    if (rtename=='NULL') {
                      rtename = "";
                    }
                    let tempdata3 = JSON.parse(sharedLib.filenametoRteWptListJSON('uploads/'+file, rtename));
                    //console.log(tempdata3);

                    if (tempdata3!==undefined && tempdata3.length!==0) {
                      for (let index3 = 0; index3 < tempdata3.length; index3++) {//for every POINT in the route

                        let item2 = tempdata3[index3];
                        //console.log(item2);
                        let wptName = 'NULL';
                        if (item2.name !='') {
                          wptName = item2.name;
                        }
                        const [rows4, fields4] = await connection.execute("SELECT COUNT(*) FROM POINT WHERE point_name='"+wptName+"' AND point_index="+item2.index+" AND latitude="+item2.lat+" AND longitude="+item2.lon+" AND route_id="+routeid+";");
                          
                        if (rows3[0]['COUNT(*)'] == 0) {//if not already in the POINT table
                          await connection.execute("INSERT INTO POINT VALUES (NULL, "+item2.index+", "+item2.lat+", "+item2.lon+", '"+wptName+"', "+routeid+");");
          
                        }
                      }
                    }

                  }
                }
              }

            }
          }
        }
      }
    }

    console.log('Storing Data to sql tables: success');
  }catch(e){
    console.log("Store Files: "+e);
    ret = 0;
  }finally {
    if (connection && connection.end) connection.end();
  }

  return ret;
}

async function clearSQLTable(dbConf) {
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;

  try{//delete all data in tables to update
    let tableCreated = 1;
    connection = await mysql.createConnection(dbConf);

    const [rows1, fields1] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'POINT';");
    if (rows1[0]['COUNT(*)'] == 0) {
      tableCreated = 0;
    }
    if (tableCreated === 1) {
      await connection.execute("delete from POINT;");
    }
    tableCreated = 1;

    const [rows2, fields2] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'ROUTE';");
    if (rows2[0]['COUNT(*)'] == 0) {
      tableCreated = 0;
    }
    if (tableCreated === 1) {
      await connection.execute("delete from ROUTE;");
    }
    tableCreated = 1;

    const [rows3, fields3] = await connection.execute("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '"+dbConf.database+"' AND table_name = 'FILE';");
    if (rows3[0]['COUNT(*)'] == 0) {
      tableCreated = 0;
    }
    if (tableCreated === 1) {
      await connection.execute("delete from FILE;");
    }
  }catch(e){
      console.log("update sql table error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  return ret;
}

app.get('/query1-sig', async function(req , res){
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;
  let retArray;

  try{
    // create the connection
    connection = await mysql.createConnection(req.query.data);
    const [rows1, fields1] = await connection.execute('SELECT * FROM ROUTE ORDER BY '+req.query.sort+';');
    //console.log(rows1);
    retArray = rows1;

  }catch(e){
      console.log("Query1 error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  
  res.send(
    {
      success: ret,
      data: retArray
    }
  )
});

app.get('/query2-sig', async function(req , res){
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;
  let retArray;

  try{
    // create the connection
    connection = await mysql.createConnection(req.query.data);
    const [rows1, fields1] = await connection.execute("SELECT * FROM ROUTE, FILE WHERE (ROUTE.gpx_id=FILE.gpx_id) AND file_name='"+req.query.filename+"' ORDER BY "+req.query.sort+";");
    //console.log(rows1);
    retArray = rows1;

  }catch(e){
      console.log("Query2 error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  
  res.send(
    {
      success: ret,
      data: retArray
    }
  )
});

app.get('/query3-sig', async function(req , res){
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;
  let retArray;

  try{
    // create the connection
    connection = await mysql.createConnection(req.query.data);
    const [rows1, fields1] = await connection.execute("SELECT * FROM POINT, ROUTE WHERE (POINT.route_id=ROUTE.route_id) AND route_name='"+req.query.route_name+"' ORDER BY point_index;");
    //console.log(rows1);
    retArray = rows1;

  }catch(e){
      console.log("Query3 error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  
  res.send(
    {
      success: ret,
      data: retArray
    }
  )
});

app.get('/query4-sig', async function(req , res){
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;
  let retArray;

  try{
    // create the connection
    connection = await mysql.createConnection(req.query.data);
    const [rows1, fields1] = await connection.execute("SELECT * FROM POINT, ROUTE, FILE WHERE (POINT.route_id=ROUTE.route_id AND ROUTE.gpx_id=FILE.gpx_id) AND file_name='"+req.query.filename+"' ORDER BY "+req.query.sort+", point_index;");
    //console.log(rows1);
    retArray = rows1;

  }catch(e){
      console.log("Query4 error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  
  res.send(
    {
      success: ret,
      data: retArray
    }
  )
});

app.get('/query5-sig', async function(req , res){
  const mysql = require('mysql2/promise');
  let connection;
  let ret = 1;
  let retArray;

  try{
    // create the connection
    connection = await mysql.createConnection(req.query.data);
    let rows1;
    let fields1;
    if (req.query.order == 'shortest') {
      [rows1, fields1] = await connection.execute("SELECT * FROM (SELECT route_id, route_name, route_len, file_name FROM ROUTE, FILE WHERE (ROUTE.gpx_id=FILE.gpx_id) AND file_name='"+req.query.filename+"' ORDER BY route_len LIMIT "+req.query.limit+") AS T ORDER BY T."+req.query.sort+";");
    } else if (req.query.order == 'longest') {
      [rows1, fields1] = await connection.execute("SELECT * FROM (SELECT route_id, route_name, route_len, file_name FROM ROUTE, FILE WHERE (ROUTE.gpx_id=FILE.gpx_id) AND file_name='"+req.query.filename+"' ORDER BY route_len DESC LIMIT "+req.query.limit+") AS T ORDER BY T."+req.query.sort+";");
    }
    //console.log(rows1);
    retArray = rows1;

  }catch(e){
      console.log("Query2 error: "+e);
      ret = 0;
  }finally {
      if (connection && connection.end) connection.end();
  }
  
  res.send(
    {
      success: ret,
      data: retArray
    }
  )
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);