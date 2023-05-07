function Login(){
    let name= $("#name").val();
    let password=$("#password").val();
    LoginAZS(name,password);
}
function ErrorAUTH(error){
    $(".stat").text("STATUS: "+error);
}