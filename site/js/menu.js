$( ".item" ).click(function() {
    $(".mark").addClass("disable");
    console.log("CLICK");
    let s;
    if(s=$(this).children(".mark")){
        s.removeClass("disable");
    }
  });
  const StatusMenu={OPENING:1,CLOSING:2,STOP:3};

  function ItemVisible(items,obj){
      if(obj.status==StatusMenu.OPENING){
          items.parentNode.style.overflowX='visible';
          items.parentNode.style.overflowY='visible';
          obj.status=StatusMenu.STOP;
      }
  }
  function ShowMenu(obj){
      
      console.log("SHOW MENU");
      obj.style.animationName='border_rev';
      obj.style.animationDuration='1s';
      obj.style.animationDirection='normal';
      obj.style.borderRadius='0px';
      let items=obj.parentNode.querySelector(".items");
      items.parentNode.status=StatusMenu.OPENING;
      items.style.left='0px';
      items.style.animationName='menuslider';
      items.style.animationDuration='1s';
      items.style.animationDirection='normal';
      setTimeout(ItemVisible, 1000,items,items.parentNode);
      
  
  }
  // function ShowMiniMenu(obj){
  //     console.log("SHOW MINI MENU");
  //     let items=obj.parentNode.querySelector(".min_menu");
      
  // }
  function HideMenu(obj){
      let listmark=$(obj).find(".mark");
      for(let i=0;i<listmark.length;i++){
          $(listmark[i]).addClass("disable");
      }
      console.log("HID MENU");
      
     
      //obj.style.overflowX='hidden';
      //obj.style.overflowY='hidden';
      obj=obj.querySelector(".items");
      obj.style.left='-100%';
      obj.style.animationName='menuslider_rev';
      obj.style.animationDuration='0.8s';
      obj.style.animationDirection='normal';
      let items=obj.parentNode.parentNode.querySelector("#min_menu");
      items.style.borderRadius='15px';
      items.style.animationName='border';
      items.style.animationDuration='0.8s';
      items.style.animationDirection='normal';
      $(obj).parent().css("overflow-x","hidden");
      $(obj).parent().css("overflow-y","hidden");
      obj.parentNode.status=StatusMenu.CLOSING;
  }
 