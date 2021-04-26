let dbConf = null;
// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    let wptList = [];
    let rteindex = 0;
    let trkindex = 0;

    // On page-load AJAX Example
    // jQuery.ajax({
    //     type: 'get',            //Request type
    //     dataType: 'json',       //Data type - we will use JSON for almost everything 
    //     url: '/endpoint1',   //The server endpoint we are connecting to
    //     data: {
    //         data1: "Value 1",
    //         data2:1234.56
    //     },
    //     success: function (data) {
    //         jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
    //         //We write the object to the console to show that the request was successful
    //         console.log(data); 

    //     },
    //     fail: function(error) {
    //         // Non-200 return, do something with error
    //         $('#blah').html("On page load, received error from server");
    //         console.log(error); 
    //     }
    // });
    
    $('#store-files').hide();
    $('#clear-all-data').hide();
    $('#display-db-status').hide();
    $('#execute-query').hide();

    // On page-load AJAX for getting the array of files objects
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint2',   //The server endpoint we are connecting to
        success: function (data) {
            if (Object.keys(data).length !== 0) {
                $('#store-files').show();
                $('#file-option').empty();
                $('#flp-table-body').empty();
                for (const key in data) {
                    $("#file-option").append('<option value="'+key+'">'+key+'</option>');
                    $("#flp-table-body").append('<tr><td><a href="'+key+'" download ">'+key+'</a></td><td>'
                        +data[key].version+'</td><td>'+data[key].creator+'</td><td>'+data[key].numWaypoints
                        +'</td><td>'+data[key].numRoutes+'</td><td>'+data[key].numTracks+'</td></tr>');
                }
            } else {
                $('#flp-table-body').empty();
                $("#flp-table-body").append('No files');
            }
            //We write the object to the console to show that the request was successful
            console.log(data); 
        },
        fail: function(error) {
            alert(error);
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#create-form').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/creategpx',   //The server endpoint we are connecting to
            data: {
                filename: $('#entryBox').val()
            },
            success: function (data) {
                filename = $('#entryBox').val();
                if (data.fail == 0) {
                    alert("file: "+filename+" created successfully.");
                    console.log("file: "+filename+" created successfully.");
                    location.reload(true);
                } else if (data.fail == 1) {
                    alert("Error: Couldn't create file: "+filename+" does not contain .gpx.");
                    console.log("Error: Couldn't create file: "+filename+" does not contain .gpx.");
                } else if (data.fail == 2) {
                    alert("Error: Couldn't create file: "+filename+" already exists.");
                    console.log("Error: Couldn't create file: "+filename+" already exists.");
                } else {
                    alert("Error: "+filename+" could not be created.");
                    console.log("Error: "+filename+" could not be created.");
                }
            },
            fail: function(error) {
                alert(error);
                console.log(error);
            }
        });
    });

    $('#gpxvp-form').submit(function(e){
        updateGPXTable();
        e.preventDefault();
    });

    $('#rename-form').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/rename',   //The server endpoint we are connecting to
            data: {
                filename: 'uploads/'+$("#file-option").val(),
                componentName: $('#rename-option').val(),
                newName: $('#rename-box').val()
            },
            success: function (data) {
                if (data.success == 1) {
                    console.log("rename successfull");
                    alert("rename successfull");
                } else {
                    console.log("Error: could not be renamed.");
                }
                updateGPXTable();

                if (dbConf !== null) {//if logged in update the mysql table
                    updateSQLTable();
                }
            },
            fail: function(error) {
                alert(error);
                console.log(error);
            }
        });
    });

    $('#details-form').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/details',   //The server endpoint we are connecting to
            data: {
                filename: 'uploads/'+$("#file-option").val(),
                componentName: $('#rename-option').val()
            },
            success: function (data) {
                console.log(data);
                if (data.success == 1) {
                    data.list.forEach(function(item, index){
                        let string = item.name + ' = ' + item.val;
                        alert(string); 
                    });
                } else {
                    console.log("Error: could not show other attributes, there may be none.");
                }
                updateGPXTable();
            },
            fail: function(error) {
                alert(error);
                console.log(error);
            }
        });
    });

    $('#add-wpt-form').submit(function(e){
        let lat = $('#wpt-Box1').val();
        let lon = $('#wpt-Box2').val();
        if ($.isNumeric(lat) == true && $.isNumeric(lon) == true) {
            let wpt = {"lat":parseFloat(lat),"lon":parseFloat(lon)};
            wptList.push(wpt);
            alert("Waypoint of lat: "+lat+", lon: "+lon+" added to Route");
        } else {
            console.log("Error: make sure values entered are numbers");
            alert("Error: make sure values entered are numbers");
        }
        e.preventDefault();
    });

    $('#add-route-form').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/addRoute',   //The server endpoint we are connecting to
            data: {
                filename: $("#file-option").val(),
                routeName: {"name": $('#route-Box').val()},
                wptlist: JSON.stringify(wptList)
            },
            success: function (data) {
                if (data.success == 1) {
                    console.log(data);
                    alert("Route added successfully");
                    if (dbConf !== null) {//if logged in update the mysql table
                        updateSQLTable();
                    }
                } else {
                    console.log("Error: route could not be added.");
                }
                updateGPXTable();
                wptList = [];
            },
            fail: function(error) {
                alert(error);
                console.log(error);
                wptList = [];
            }
        });
    });

    $('#path-form').submit(function(e){
        let lat1 = $("#sp-Box1").val();
        let lon1 = $("#sp-Box2").val();
        let lat2 = $("#ep-Box1").val();
        let lon2 = $("#ep-Box2").val();
        let acc = $("#acc-Box").val();
        
        e.preventDefault();
        if ($.isNumeric(lat1) == true && $.isNumeric(lon1) == true && $.isNumeric(lat2) == true && $.isNumeric(lon2) == true && $.isNumeric(acc) == true ) {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/findpath',   //The server endpoint we are connecting to
                data: {
                    spLat: parseFloat(lat1),
                    spLon: parseFloat(lon1),
                    epLat: parseFloat(lat2),
                    epLon: parseFloat(lon2),
                    accuracy: parseFloat(acc),
                },
                success: function (data) {
                    $("#fp-table-body").empty();
                    if (data.routes!==undefined && data.routes.length!==0) {
                        data.routes.forEach(function(item, index){
                            let ind = index+1;
                            $("#fp-table-body").append('<tr><td>Route '+(index+1)+'</td><td>'+item.name+
                                '</td><td>'+item.numPoints+'</td><td>'+item.len+'m</td><td>'+item.loop+'</td></tr>');
                        });
                    } else {
                        $("#fp-table-body").append('<h6>No Routes</h6>');
                    }
        
                    if (data.tracks!==undefined && data.routes.length!==0) {
                        data.tracks.forEach(function(item, index){
                            $("#fp-table-body").append('<tr><td>Track '+(index+1)+'</td><td>'+item.name+
                                '</td><td>'+item.numPoints+'</td><td>'+item.len+'m</td><td>'+item.loop+'</td></tr>');
                        });
                    } else {
                        $("#fp-table-body").append('<h6>No Tracks</h6>');
                    }
                    //We write the object to the console to show that the request was successful
                    console.log(data); 
                },
                fail: function(error) {
                    alert(error);
                    console.log(error); 
                }
            });
        } else {
            console.log("Error: make sure values entered are numbers");
            alert("Error: make sure values entered are numbers");
        }
    });

    //Login to mysql
    $('#login-form').submit(function(e){
        e.preventDefault();
        dbConf = {
            host: 'dursley.socs.uoguelph.ca',
            user: $('#login-username-Box').val(),
            password: $('#login-password-Box').val(),
            database: $('#login-database-name-Box').val()
        };
        $.ajax({
            type: 'get',
            dataType: 'json', 
            url: '/login',
            data: dbConf,
            success: function (data) {
                if (data.success == 1) {
                    if ($('#display-db-status').is(':hidden')) {
                        $('#display-db-status').show();
                    }
                    if ($('#execute-query').is(':hidden')) {
                        $('#execute-query').show();
                        //hide queries in the div
                        $('#query1').hide();
                        $('#query2').hide();
                        $('#query3').hide();
                        $('#query4').hide();
                        $('#query5').hide();
                    }
                    displayDBStatus();
                } else {
                    alert('Connection Failed\nPLease re-enter the username, DB name and password and try again.');
                }
                console.log(data);
            },
            fail: function(error) {
                alert(error);
                console.log(error); 
            }
        });
    });

    //to store file data into mysql tables
    $('#store-files-button').click(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json', 
            url: '/store-files',
            data: {
                data: dbConf
            },
            success: function (data) {
                if (data.success == 1) {
                    displayDBStatus();
                } else {
                    alert('Connection Failed\nPLease re-enter the username, DB name and password and try again.');
                }
                console.log(data);
            },
            fail: function(error) {
                alert(error);
                console.log(error); 
            }
        });
    });

    //to display status of mysql tables
    $('#display-db-status-button').click(function(e){
        e.preventDefault();
        displayDBStatus();
    });

    //to clear all data from tables
    $('#clear-all-data-button').click(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/clear-sql-table',
            data: {
                data: dbConf
            },
            success: function (data) {
                if (data.success == 1) {
                    displayDBStatus();
                    $('#clear-all-data').hide();
                } else {
                    alert('Connection Failed\nPLease re-enter the username, DB name and password to update table');
                }
                console.log(data);
            },
            fail: function(error) {
                alert(error);
                console.log(error); 
            }
        });
    });

    $('#query-select-form').submit(function(e){
        e.preventDefault();
        $('#query1').hide();
        $('#query2').hide();
        $('#query3').hide();
        $('#query4').hide();
        $('#query5').hide();
        $( '#'+$('#query-option').val() ).show();
    });

    $('#query1-form').submit(function(e){
        e.preventDefault();
        let order;
        if ($('#query1-Box').val() === 'name') {
            order = 'route_name';
        } else if ($('#query1-Box').val() === 'length') {
            order = 'route_len';
        } else {
            order = 0;
        }

        if (order !== 0) {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/query1-sig',
                data: {
                    data: dbConf,
                    sort: order
                },
                success: function (data) {
                    if (data.success === 1) {
                        $("#query1-table-body").empty();
                        data.data.forEach(function(item){
                            $("#query1-table-body").append("<tr><td>"+item.route_id+"</td><td>"+item.route_name
                                +"</td><td>"+item.route_len+"</td><td>"+item.gpx_id+"</td></tr>");
                        });
                    } else {
                        alert("Connection Failed\nPLease re-enter the username, DB name and password to update table.");
                    }
                    console.log(data);
                },
                fail: function(error) {
                    alert(error);
                    console.log(error); 
                }
            });
        } else {
            alert("Invalid sort order\nPlease enter 'name' or 'length'");
        }
    });

    $('#query2-form').submit(function(e){
        e.preventDefault();
        let order;
        if ($('#query2-Box2').val() === 'name') {
            order = 'route_name';
        } else if ($('#query2-Box2').val() === 'length') {
            order = 'route_len';
        } else {
            order = 0;
        }

        if (order !== 0) {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/query2-sig',
                data: {
                    data: dbConf,
                    sort: order,
                    filename: $('#query2-Box1').val()
                },
                success: function (data) {
                    if (data.success === 1) {
                        $("#query2-table-body").empty();
                        data.data.forEach(function(item){
                            $("#query2-table-body").append("<tr><td>"+item.route_id+"</td><td>"+item.route_name
                            +"</td><td>"+item.route_len+"</td><td>"+item.gpx_id+"</td><td>"+item.file_name+"</td></tr>");
                        });
                    } else {
                        alert("Connection Failed\nPLease re-enter the username, DB name and password to update table.");
                    }
                    console.log(data);
                },
                fail: function(error) {
                    alert(error);
                    console.log(error); 
                }
            });
        } else {
            alert("Invalid sort order\nPlease enter 'name' or 'length'");
        }
    });

    $('#query3-form').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/query3-sig',
            data: {
                data: dbConf,
                route_name: $('#query3-Box').val()
            },
            success: function (data) {
                if (data.success === 1) {
                    $("#query3-table-body").empty();
                    data.data.forEach(function(item){
                        $("#query3-table-body").append("<tr><td>"+item.point_id+"</td><td>"+item.point_index
                            +"</td><td>"+item.latitude+"</td><td>"+item.longitude+"</td><td>"+item.point_name
                            +"</td><td>"+item.route_id+"</td><td>"+item.route_name+"</td></tr>");
                    });
                } else {
                    alert("Connection Failed\nPLease re-enter the username, DB name and password to update table.");
                }
                console.log(data);
            },
            fail: function(error) {
                alert(error);
                console.log(error); 
            }
        });
    });

    $('#query4-form').submit(function(e){
        e.preventDefault();
        let order;
        if ($('#query4-Box2').val() === 'name') {
            order = 'route_name';
        } else if ($('#query4-Box2').val() === 'length') {
            order = 'route_len';
        } else {
            order = 0;
        }

        if (order !== 0) {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/query4-sig',
                data: {
                    data: dbConf,
                    sort: order,
                    filename: $('#query4-Box1').val()
                },
                success: function (data) {
                    if (data.success === 1) {
                        let i=0;
                        let unName;
                        $("#query4-table-body").empty();
                        data.data.forEach(function(item){
                            console.log(item.route_name);
                            if (item.route_name=='NULL') {
                                if (item.point_index==0) {
                                    i++;
                                    unName = 'Unnamed Route '+i;
                                }
                                $("#query4-table-body").append("<tr><td>"+item.point_id+"</td><td>"+item.point_index+"</td><td>"+item.latitude
                                    +"</td><td>"+item.longitude+"</td><td>"+item.point_name+"</td><td>"+item.route_id+"</td><td>"+unName
                                    +"</td><td>"+item.route_len+"</td><td>"+item.gpx_id+"</td><td>"+item.file_name+"</td></tr>");
                            } else {
                                $("#query4-table-body").append("<tr><td>"+item.point_id+"</td><td>"+item.point_index+"</td><td>"+item.latitude
                                    +"</td><td>"+item.longitude+"</td><td>"+item.point_name+"</td><td>"+item.route_id+"</td><td>"+item.route_name
                                    +"</td><td>"+item.route_len+"</td><td>"+item.gpx_id+"</td><td>"+item.file_name+"</td></tr>");
                            }
                        });
                    } else {
                        alert("Connection Failed\nPLease re-enter the username, DB name and password to update table.");
                    }
                    console.log(data);
                },
                fail: function(error) {
                    alert(error);
                    console.log(error); 
                }
            });
        } else {
            alert("Invalid sort order\nPlease enter 'name' or 'length'");
        }
    });

    $('#query5-form').submit(function(e){
        e.preventDefault();
        let order;
        if ($('#query5-Box4').val() === 'name') {
            order = 'route_name';
        } else if ($('#query5-Box4').val() === 'length') {
            order = 'route_len';
        } else {
            order = 0;
        }
        if (order != 0 && ($('#query5-Box2').val() == 'shortest' || $('#query5-Box2').val() == 'longest') && $.isNumeric($('#query5-Box1').val())) {
            let num = parseInt($('#query5-Box1').val());
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/query5-sig',
                data: {
                    data: dbConf,
                    order: $('#query5-Box2').val(),
                    filename: $('#query5-Box3').val(),
                    sort: order,
                    limit: num
                },
                success: function (data) {
                    if (data.success === 1) {
                        $("#query5-table-body").empty();
                        for (let index = 0; index < data.data.length; index++) {
                            item=data.data[index];
                            $("#query5-table-body").append("<tr><td>"+item.route_id+"</td><td>"+item.route_name+"</td><td>"+item.route_len
                                +"</td><td>"+item.file_name+"</td></tr>");
                        }
                    } else {
                        alert("Connection Failed\nPLease re-enter the username, DB name and password to update table.");
                    }
                    console.log(data);
                },
                fail: function(error) {
                    alert(error);
                    console.log(error); 
                }
            });
        } else {
            alert("Invalid sort, order or number\nPlease enter 'shortest' or 'longest' and 'name'or 'length'");
        }
        
    });

});

function updateGPXTable(){
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/openfile',
        data: {
            filename: $("#file-option").val()
        },
        success: function (data) {
            $("#gpx-table-body").empty();
            $("#rename-option").empty();
            $("#detail-option").empty();
            if (data.routes!==undefined && data.routes.length!==0) {
                data.routes.forEach(function(item, index){
                    let ind = index+1;
                    rteindex = ind;
                    $("#gpx-table-body").append('<tr><td>Route '+(index+1)+'</td><td>'+item.name+
                        '</td><td>'+item.numPoints+'</td><td>'+item.len+'m</td><td>'+item.loop+'</td></tr>');
                        $("#rename-option").append('<option value="'+item.name+'">'+item.name+'</option>');
                        $("#detail-option").append('<option value="'+item.name+'">'+item.name+'</option>');
                });
            } else {
                $("#gpx-table-body").append('<h6>No Routes</h6>');
            }

            if (data.tracks!==undefined && data.tracks.length!==0) {
                data.tracks.forEach(function(item, index){
                    let ind = index+1;
                    trkindex = ind;
                    $("#gpx-table-body").append('<tr><td>Track '+(index+1)+'</td><td>'+item.name+
                        '</td><td>'+item.numPoints+'</td><td>'+item.len+'m</td><td>'+item.loop+'</td></tr>');
                        $("#rename-option").append('<option value="'+item.name+'">'+item.name+'</option>');
                        $("#detail-option").append('<option value="'+item.name+'">'+item.name+'</option>');
                });
            } else {
                $("#gpx-table-body").append('<h6>No Tracks</h6>');
            }
            //We write the object to the console to show that the request was successful
            console.log(data); 
        },
        fail: function(error) {
            alert(error);
            console.log(error); 
        }
    });
}

function displayDBStatus() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/display-DB-Status',
        data: {
            data: dbConf
        },
        success: function (data) {
            alert(data.string);
            console.log(data.string);
            if ($('#clear-all-data').is(':hidden') && (data.numFiles > 0 || data.numRoutes > 0 || data.numPoints > 0)) {
                $('#clear-all-data').show();
            }
        },
        fail: function(error) {
            alert(error);
            console.log(error); 
        }
    });
}

function updateSQLTable() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/update-sql-table',
        data: {
            data: dbConf
        },
        success: function (data) {
            if (data.success == 1) {
                displayDBStatus();
            } else {
                alert('Connection Failed\nPLease re-enter the username, DB name and password to update table.');
            }
            console.log(data);
        },
        fail: function(error) {
            alert(error);
            console.log(error); 
        }
    });
}