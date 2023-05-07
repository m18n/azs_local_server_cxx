
function auth(){
    var formData =JSON.stringify($('#auth :input').serializeArray());
   
    console.log(formData+"\n");
    $.ajax({
        type: "POST",
        url: "/auth",
        data: formData,
        success: function(data){
            console.log(data);
        },
        dataType: "json",
        contentType : "application/json"
    });
}