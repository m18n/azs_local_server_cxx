let col_item=0;
$(".lable_trk").click(function(){
    let item=$(this).parents(".item_trk");
    let viewport=$(item).parents(".viewport");
    let gui=$(item).find(".guitrk")
    let index=$(".viewport .item_trk").index(item);
    let px=index*50;
    console.log("PX: "+px);
    if($(gui).hasClass("visible")){
        $(gui).removeClass("visible");
        $(viewport).css({"transform":"translateY(0px)"});
    }else{
      
            $(viewport).children('.item_trk').each(function(index) {
                $(this).find(".guitrk").removeClass("visible");
              });
        
        $(gui).addClass("visible");
        $(viewport).css({"transform":"translateY(-"+px+"px)"});
        
    }
    
});
$(document).ready(function () {
    col_item=$('.viewport .item_trk').length;
    console.log(col_item);
    console.log($(".viewport").child)
});