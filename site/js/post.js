
function auth(){
    var formData =JSON.stringify($('#auth :input').serializeArray());
    console.log(formData+"\n");
    $.ajax({
        type: "POST",
        url: "/auth",
        data: formData,
        success: function(data){
            if(data["status"]=="yes"){
                document.location.href = '/main';
            }else{
                $(".status").text("STATUS: wrong password");
            }
        },
        dataType: "json",
        contentType : "application/json"
    });
}

function settings_db_check(){
    console.log("ajax");
    $.ajax({
        type: "GET",
        url: "/settings/dberror/check",
        success: function(data){
            console.log("DATA: "+data);
            console.log("STATUS:"+data["status"]);
            if(data["status"]=="process"){
                $("#stat_con").text("STATUS CONNECT: process");
                $("#stat_con").attr("xdata","1")
               
               // document.location.href="/main/settings/database";
            }else if(data["status"]=="connect"){
                $("#stat_con").text("STATUS CONNECT: connect");
                $("#stat_con").attr("xdata","0")
                
            }else if(data["status"]=="disconnect"){
                $("#stat_con").text("STATUS CONNECT: disconnect");
                $("#stat_con").attr("xdata","-1")
                
            }
        },
        error: function(data){  
            document.location.href="/settings/dberror";
        },
        dataType: "json",
        contentType : "application/json"
    });
    
}
function settings_db_error(){
    var formData =JSON.stringify($('#sett_azs').find(':input').serializeArray());
    console.log(formData+"\n");
    $.ajax({
        type: "POST",
        url: "/settings/dberror/send",
        data: formData,
        success: function(data){
            if(data["status"]=="ok"){
                
                document.location.href="/settings/dberror";
            }else{
                console.log("Error send");
            }
        },
        
        dataType: "json",
        contentType : "application/json"
    });
   
}

function save_pump(json_object){
    $.ajax({
        type: "POST",
        url: "/api/pump/save",
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        data: json_object,
        success: function(data){console.log(data)},
        error: function(errMsg) {
            console.log(data)
        }
        
    });
}
