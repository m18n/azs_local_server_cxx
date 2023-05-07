const Regim = { Resize: 0, Move: 1,ResizeState:2,State:3};
function CreateUnit(){
    var unit={
        'id':0,
        'width':0,
        'heigth':0,
        'x':0,
        'y':0,
    }
    return unit;
}
function RegimeResizeUnit(obj){
    
    if(obj.Regim==undefined){
        obj.Regim=Regim.State;
    }
    console.log("OBJ REGIM "+ obj.Regim);
    if(obj.Regim==Regim.State){
        obj.Regim=Regim.ResizeState;
        obj.style.borderColor="green"; 
    }else if(obj.Regim==Regim.ResizeState){
        obj.Regim=Regim.State;
        obj.style.borderColor="grey";
        LOG("OBJ ID:"+obj.iddata);
        SaveResize(obj.iddata,obj.offsetWidth,obj.offsetHeight);
        const domRect = obj.getBoundingClientRect();
        console.log(domRect);
    }
}
function MoveMouse(event,obj){
 
    if(obj.Regim==Regim.Resize){
        let cord = obj.getBoundingClientRect();
        let x=event.clientX-cord.left;
        let y=event.clientY-cord.top;
        console.log("OLXDW: "+obj.startsizex);
        obj.style.width=obj.startsizex+(x-obj.xstart)+"px";
        obj.style.height=obj.startsizey+(y-obj.ystart)+"px";
        console.log("EDIT SIZE X: +"+(x-obj.xstart)+" Y: "+(y-obj.ystart)+" ID: "+obj.id);
    }else if(obj.Regim==Regim.Move){
        MoveObj(event,obj);
    }
    
}
function StartMoveObj(event,obj){
    if(obj.brs==undefined){
        obj.brs=false;
    }
    let cord = obj.getBoundingClientRect();
    obj.xstartmove=event.clientX;
    obj.ystartmove=event.clientY;
    obj.locx=cord.left;
    obj.locy=cord.top;
    console.log("START MOVE X: "+obj.xstartmove+" MOVE Y: "+obj.ystartmove+" ID: "+obj.id);
    obj.Regim=Regim.Move;
    obj.style.cursor='move';
}
function StopMoveObj(event,obj){
    let x=event.clientX;
    let y=event.clientY;
    console.log("STOP MOVE X: "+x+" MOVE Y: "+y+" ID: "+obj.id);
    obj.Regim=Regim.State;
    obj.style.cursor="default";
}
function MoveObj(event,obj){
   
        let cord = obj.getBoundingClientRect();
        let x=event.clientX;
        let y=event.clientY;
        console.log("LOC X: "+obj.locx+" LOC Y: "+obj.locy);
        console.log("CLIENT X: "+x+" CLINET Y: "+y);
        obj.style.top=obj.locy+(y-obj.ystartmove)+"px";
        obj.style.left=obj.locx+(x-obj.xstartmove)+"px";
    
}
let boolmouse;
function RegimMove(event,obj){
    if(obj.Regim==Regim.State&&boolmouse==true){
       
            obj.style.borderColor="red";
            StartMoveObj(event,obj);
        
    }
}
function ResizeStartUnit(event,obj){
    boolmouse=true;
    if(obj.Regim==undefined){
        obj.Regim=Regim.State;
    }
    console.log("REGIM :"+obj.Regim);
    if(obj.Regim==Regim.ResizeState){
        obj.Regim=Regim.Resize;
        let cord = obj.getBoundingClientRect();
        obj.xstart=event.clientX-cord.left;
        obj.ystart=event.clientY-cord.top;
        obj.startsizex=obj.clientWidth;
        obj.startsizey=obj.clientHeight;
        obj.style.cursor="all-scroll";        
    }else if(obj.Regim==Regim.State){
        setTimeout(RegimMove, 3000,event,obj); 
    }else if(obj.Regim==Regim.Move){
       
        obj.style.borderColor="grey";
        StopMoveObj(event,obj);
    }
}
function ResizeStopUnit(event,obj){
    
    boolmouse=false;
    obj.style.cursor="default";
    console.log("REGIME START: "+obj.Regim);
    if(obj.Regim==Regim.Resize){
        obj.Regim=Regim.ResizeState;
    }else if(obj.Regim==Regim.Move){
        let cord = obj.getBoundingClientRect();
        
            LOG("SAVE MOVE");
            SaveMove(obj.iddata,cord.left,cord.top);
        
        obj.Regim=Regim.State;
        obj.style.borderColor="grey";
        StopMoveObj(event,obj);
        
        
        
    }
    
    console.log("REGIME END: "+obj.Regim);
}

function Enter(input){
    console.log("ENTER");
    CalGasolineLiter(input);
}
function CalGasolineMany(input){
   
    //let price=document.getElementById("price").value;
    let price=input.parentNode.parentNode.querySelector("#price");
    let liter=input.parentNode.querySelector('#liter');
    console.log("LITER VALUE: "+liter.value+" PRICE: "+price.value);
    liter.value=((input.value/price.value)-0.005).toFixed(2);
    
}
function CalGasolineLiter(input){
    
    //let price=document.getElementById("price").value;
    let price=input.parentNode.parentNode.querySelector("#price");
    let liter=input.parentNode.querySelector('#liter');
    let many=input.parentNode.querySelector('#many');
    console.log("LITER VALUE: "+liter.value+" PRICE: "+price.value);
    many.value=(input.value*price.value).toFixed(2);
}

function GetVar(data){
    let variable=data.split('|');
    var arr=[];
    for(let i=0;i<variable.length;i++){
        let vare=variable[i].split(':');
        
        var v={
            'name':vare[0],
            'value':vare[1],
        };
        arr.push(v);
    }
    return arr;
}

function LoadUnit(unit){
    let df=GetVar(unit);
    let obj=$("#unit_shab").clone();
    obj.attr("id",df[0].value);
    obj.removeClass("none");
    //LOG("LEFT: "+df[3].value+" TOP: "+df[4].value);
    obj.css("width",df[1].value+"px");
    obj.css("height",df[2].value+"px");
    obj.css("left",df[3].value+"px");
    obj.css("top",df[4].value+"px");
    

    console.log("VALUE: "+df[0].value);
    obj.appendTo(".content");
    obj.find(".item_g").on("click",OpenGlav);
    obj.find(".item_o").on("click",OpenOpt);
    let domelem=obj.get(0);
    domelem.iddata=df[0].value;
}
String.prototype.replaceAt = function(index, replacement) {
    return this.substring(0, index) + replacement + this.substring(index + replacement.length);
}
function OnNext(th){
    console.log("$$$");
    let row1=$(th).parent().parent().parent();
    let liter=$(row1).find(".liter").get(0);

    CalGasolineLiter(liter);
}
function OnCal(th){
    let row1=$(th).parent().parent().parent();
    let inputfocus=null;
    let b=false;
    let inputnotfocus=null;
    let calculator=$(th).parent().parent();
    if(calculator.attr('id')=='cl_red'){
        inputfocus=$(row1).find(".many");
        inputnotfocus=$(row1).find(".liter");
    }else{
        b=true;
        inputfocus=$(row1).find(".liter");
        inputnotfocus=$(row1).find(".many");
        console.log("LITER");
    }
    if(calculator.get(0).oncal==false){
       
        inputfocus.val(".00");
        inputnotfocus.val("0.00");
        calculator.get(0).oncal=true;
        calculator.get(0).regimdrob=-1;
       
    }
    let mynumber=$(th).text();
    let valueinput=inputfocus.val();
  
    if(mynumber=='.'){
        calculator.get(0).regimdrob=0;
        return;
    }
    let index=valueinput.indexOf(".");
    let r=calculator.get(0).regimdrob;
    if(r==-1){
        let newstr=valueinput.slice(0,index)+mynumber+".00";
        inputfocus.val(newstr);
        
    }else{
        if(r<2){
            valueinput= valueinput.replaceAt(index+1+r,mynumber);
            console.log("REEGIM "+valueinput);
            inputfocus.val(valueinput);
            calculator.get(0).regimdrob+=1;
        }
    }
    if(b==false){
        CalGasolineMany(inputfocus.get(0));
    }else{
        CalGasolineLiter(inputfocus.get(0));
    }
  };
function checkBack(event){
    console.log("EVENT: "+event);
}
function OpenClavaRed(el){
    let cal=$(el).parent().parent();
    let many=cal.children(".input_des").children("#many");
    let liter=cal.children(".input_des").children("#liter");
    many.addClass("focus");
    liter.removeClass("focus");
    many.val(null);
    
    liter.val(null);
    cal.children("#cl_blue").addClass("none");
    cal.children("#out_des").addClass("none");
    let cl_red=cal.children("#cl_red");
    cl_red.removeClass("none");
    cl_red.get(0).oncal=false;
    // $("#cl_blue").addClass("none");
    // $("#out_des").addClass("none");
    // $("#cl_red").removeClass("none");
}
function OpenClavaBlue(el){
    let cal=$(el).parent().parent();
    let many=cal.children(".input_des").children("#many");
    let liter= cal.children(".input_des").children("#liter");
    many.removeClass("focus");
    liter.addClass("focus");
    many.val(null);
    
    liter.val(null);
    cal.children("#cl_red").addClass("none");
    cal.children("#out_des").addClass("none");
    let cl_blue=cal.children("#cl_blue");
    cl_blue.removeClass("none");
    cl_blue.get(0).oncal=false;
}
function StartDes(th){
    let content= $(th).parent().parent();
    content.find(".clava").addClass("none");
    content.find(".out_des").removeClass("none");
    let info_ges=content.find(".info_ges");
    info_ges.text("РОБОТА");
    info_ges.css("background-color","#dbdb18");
}

function StopDes(th){
    let content= $(th).parent().parent();
    let info_ges=content.find(".info_ges");
    info_ges.text("ОСТАНОВ");
    info_ges.css("background-color","green");
}