
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
function settings_db(){
    var formData =JSON.stringify($('#sett_azs').find(':input').serializeArray());
    console.log(formData+"\n");
    $.ajax({
        type: "POST",
        url: "/settings/db",
        data: formData,
        success: function(data){
            if(data["status"]=="yes"){
                document.location.href = '/';
            }
        },
        dataType: "json",
        contentType : "application/json"
    });
    
}
function save_scale_pump(id,scale){
    let savescale={id:id,scale:scale}
    let json = JSON.stringify(savescale);
    console.log("JSON: "+json);
    $.ajax({
        type: "POST",
        url: "/api/pump/savescale",
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        data: json,
        success: function(data){console.log(data)},
        error: function(errMsg) {
            console.log(data)
        }
        
    });
}