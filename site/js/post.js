
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