const Regim = { Resize: 0, Move: 1, ResizeState: 2, State: 3 };
function CreateUnit() {
    var unit = {
        'id': 0,
        'width': 0,
        'heigth': 0,
        'x': 0,
        'y': 0,
    }
    return unit;
}
function spacebig(space) {
    
    $(space).css({ "width": "500%", "height": "500%" });
    $(space).css("transform", "translate(-50%, -50%)");
}
function spacesmall(space) {
    $(space).css({ "width": "100%", "height": "100%" });
    $(space).css("transform", "translate(-100%, -100%)");
}
let boolmouse;
// REG_RESIZE REG_RESIZE REG_RESIZE REG_RESIZE
function REG_RESIZE_ENTER(obj, space) {
    if (obj.Regim == Regim.State) {
        let width_unit = $(obj).width();
        spacebig(space);
        obj.style.borderColor = "green";
        obj.Regim = Regim.ResizeState;
        return true;
    }
    return false;
}
function REG_RESIZE_START(event, obj) {
    if (obj.Regim == Regim.ResizeState) {
        let cord = obj.getBoundingClientRect();
        obj.xstart = event.clientX - cord.left;
        obj.ystart = event.clientY - cord.top;
        console.log("OBJ WIDTH CL: " + obj.xstart + " OBJ HEIGTH CL: " + obj.ystart + "\n");
        obj.firstwidth = $(obj).width();
        obj.firstheight = $(obj).height();
        obj.style.cursor = "all-scroll";
        obj.Regim = Regim.Resize;
        return true;
    }
    return false;
}
function REG_RESIZE_MOVE(event, obj) {
    if (obj.Regim == Regim.Resize && boolmouse == true) {
        let cord = obj.getBoundingClientRect();
        let x = event.clientX - cord.left;
        let y = event.clientY - cord.top;
        let xy = (x - obj.xstart + y - obj.ystart) / 2;
        $(obj).width(obj.firstwidth + xy + "px");
        $(obj).height( obj.firstheight + xy + "px");
        // obj.style.width = obj.firstwidth + xy + "px";
        // obj.style.height = obj.firstheight + xy + "px";
        return true;
    }
    return false;
}

function REG_RESIZE_STOP(obj) {
    if (obj.Regim == Regim.Resize) {
        obj.Regim = Regim.ResizeState;
        return true;
    }
    return false;
}
function REG_RESIZE_ESCAPE(obj, space) {
    if (obj.Regim == Regim.ResizeState) {
        obj.style.borderColor = "grey";
        spacesmall(space);
        let scale = ($(obj).width() + 8) / 300 * 100;
        save_scale_pump($(obj).attr('value'), scale);
        obj.Regim = Regim.State;
        return true;
    }
    return false;
}
// REG_MOVE REG_MOVE REG_MOVE REG_MOVE


function REG_MOVE_STARTTIMER(event, obj) {
    if (obj.Regim == Regim.State) {
        setTimeout(REG_MOVE_START, 3000, event, obj);
        return true;
    }
    return false;

}

function REG_MOVE_START(event, obj) {
    if (obj.Regim == Regim.State && boolmouse == true) {
        obj.style.borderColor = "red";
        let space=$(obj).find(".space")[0];
        spacebig(space);
        let rect = obj.getBoundingClientRect();
        obj.offsetx=event.offsetX;
        obj.offsety=event.offsetY;
        obj.initialX=obj.offsetLeft-event.pageX;
        obj.initialY=obj.offsetTop-event.pageY;
        console.log("OFSET X: "+event.offsetX+" OFSET Y: "+event.offsetY);
        $(".cord").text("START PAGEX:"+event.pageX+" PAGEY: "+event.pageY+" RECT T:"+rect.top+" RECT LEFT: "+rect.left);
        console.log("Start MOVE X: " + event.pageX + " MOVE Y: " + event.pageY + " ID: " + obj.id);
        obj.style.cursor='move';
        obj.Regim = Regim.Move;
        return true;
    }
    return false;
}
function REG_MOVE_MOVEOBJ(event, obj) {
    if (obj.Regim == Regim.Move && boolmouse == true) {
        let rect = obj.getBoundingClientRect();
        $(".cord2").text("POSLE MOVE PAGEX:"+event.pageX+" PAGEY: "+event.pageY+" RECT T:"+rect.top+" RECT LEFT: "+rect.left);
        currentX = event.pageX+obj.initialX;
        currentY = event.pageY+obj.initialY;
        console.log("OFSET X: "+event.offsetX+" OFSET Y: "+event.offsetY);
        
        console.log("MOUSE: X: "+event.pageX+" Y: "+event.pageY);
        obj.style.top = currentY + "px";
        obj.style.left = currentX + "px";
      
        return true;
    }
    return false;
}

function REG_MOVE_STOP(event, obj) {

    if (obj.Regim == Regim.Move) {
        $(".cord2").text("(STOP)"+$(".cord2").text());
        $(".cord").text("(STOP)"+$(".cord").text());
        let x = event.clientX;
        let y = event.clientY;
        spacesmall($(obj).find(".space")[0]);
        console.log("STOP MOVE X: " + x + " MOVE Y: " + y + " ID: " + obj.id);
        obj.style.borderColor = "grey";
        obj.style.cursor = "default";
        obj.Regim = Regim.State;
        return true;
    }
    return false;
}

// MOUSE MOUSE MOUSE MOUSE
function RegimeResizeUnit(obj) {
    let space = obj;
    obj = $(obj).parents(".dispensing_unit")[0];
    if (obj.Regim == undefined) {
        obj.Regim = Regim.State;
    }
    console.log("OBJ REGIM " + obj.Regim);
    if (REG_RESIZE_ENTER(obj, space)) { }
    else if (REG_RESIZE_ESCAPE(obj, space)) { }

}
$('.space').mousedown(function(event){
    obj = $(this).parents(".dispensing_unit")[0];
    boolmouse = true;
    if (obj.Regim == undefined) {
        obj.Regim = Regim.State;
        console.log("UNFIDER\n");
    }
    console.log("REGIM :" + obj.Regim);
    if (REG_RESIZE_START(event, obj)) { }
    else if (REG_MOVE_STARTTIMER(event, obj)) { }
});
$('.space').mouseup(function(e){
    MouseUp(e,$(this));
})
function MouseUp(event, obj) {
    obj = $(obj).parents(".dispensing_unit")[0];
    boolmouse = false;
    obj.style.cursor = "default";
    console.log("REGIME START: " + obj.Regim);
    if (REG_RESIZE_STOP(obj)) { }
    else if (REG_MOVE_STOP(event, obj)) { }
    console.log("REGIME END: " + obj.Regim);
}
$(".space").mousemove(function(event){
    MoveMouse(event,$(this));
});
function MoveMouse(event, obj) {
    obj = $(obj).parents(".dispensing_unit")[0];
    if (REG_RESIZE_MOVE(event, obj)) { }
    else if (REG_MOVE_MOVEOBJ(event, obj)) { }
}
function MouseLeave(event, obj) {
    console.log("mouse leave");
    MouseUp(event, obj);
}


// PUMP PUMP PUMP PUMP
function Enter(input) {
    console.log("ENTER");
    CalGasolineLiter(input);
}
function CalGasolineMany(input) {

    //let price=document.getElementById("price").value;
    let price = input.parentNode.parentNode.querySelector("#price");
    let liter = input.parentNode.querySelector('#liter');
    console.log("LITER VALUE: " + liter.value + " PRICE: " + price.value);
    liter.value = ((input.value / price.value) - 0.005).toFixed(2);

}
function CalGasolineLiter(input) {

    //let price=document.getElementById("price").value;
    let price = input.parentNode.parentNode.querySelector("#price");
    let liter = input.parentNode.querySelector('#liter');
    let many = input.parentNode.querySelector('#many');
    console.log("LITER VALUE: " + liter.value + " PRICE: " + price.value);
    many.value = (input.value * price.value).toFixed(2);
}

function GetVar(data) {
    let variable = data.split('|');
    var arr = [];
    for (let i = 0; i < variable.length; i++) {
        let vare = variable[i].split(':');

        var v = {
            'name': vare[0],
            'value': vare[1],
        };
        arr.push(v);
    }
    return arr;
}

function LoadUnit(unit) {
    let df = GetVar(unit);
    let obj = $("#unit_shab").clone();
    obj.attr("id", df[0].value);
    obj.removeClass("none");
    //LOG("LEFT: "+df[3].value+" TOP: "+df[4].value);
    obj.css("width", df[1].value + "px");
    obj.css("height", df[2].value + "px");
    obj.css("left", df[3].value + "px");
    obj.css("top", df[4].value + "px");


    console.log("VALUE: " + df[0].value);
    obj.appendTo(".content");
    obj.find(".item_g").on("click", OpenGlav);
    obj.find(".item_o").on("click", OpenOpt);
    let domelem = obj.get(0);
    domelem.iddata = df[0].value;
}
String.prototype.replaceAt = function (index, replacement) {
    return this.substring(0, index) + replacement + this.substring(index + replacement.length);
}
function OnNext(th) {
    console.log("$$$");
    let row1 = $(th).parent().parent().parent();
    let liter = $(row1).find(".liter").get(0);

    CalGasolineLiter(liter);
}
function OnCal(th) {
    let row1 = $(th).parent().parent().parent();
    let inputfocus = null;
    let b = false;
    let inputnotfocus = null;
    let calculator = $(th).parent().parent();
    if (calculator.attr('id') == 'cl_red') {
        inputfocus = $(row1).find(".many");
        inputnotfocus = $(row1).find(".liter");
    } else {
        b = true;
        inputfocus = $(row1).find(".liter");
        inputnotfocus = $(row1).find(".many");
        console.log("LITER");
    }
    if (calculator.get(0).oncal == false) {

        inputfocus.val(".00");
        inputnotfocus.val("0.00");
        calculator.get(0).oncal = true;
        calculator.get(0).regimdrob = -1;

    }
    let mynumber = $(th).text();
    let valueinput = inputfocus.val();

    if (mynumber == '.') {
        calculator.get(0).regimdrob = 0;
        return;
    }
    let index = valueinput.indexOf(".");
    let r = calculator.get(0).regimdrob;
    if (r == -1) {
        let newstr = valueinput.slice(0, index) + mynumber + ".00";
        inputfocus.val(newstr);

    } else {
        if (r < 2) {
            valueinput = valueinput.replaceAt(index + 1 + r, mynumber);
            console.log("REEGIM " + valueinput);
            inputfocus.val(valueinput);
            calculator.get(0).regimdrob += 1;
        }
    }
    if (b == false) {
        CalGasolineMany(inputfocus.get(0));
    } else {
        CalGasolineLiter(inputfocus.get(0));
    }
};
function checkBack(event) {
    console.log("EVENT: " + event);
}
function OpenClavaRed(el) {
    let cal = $(el).parent().parent();
    let many = cal.children(".input_des").children("#many");
    let liter = cal.children(".input_des").children("#liter");
    many.addClass("focus");
    liter.removeClass("focus");
    many.val(null);

    liter.val(null);
    cal.children("#cl_blue").addClass("none");
    cal.children("#out_des").addClass("none");
    let cl_red = cal.children("#cl_red");
    cl_red.removeClass("none");
    cl_red.get(0).oncal = false;
    // $("#cl_blue").addClass("none");
    // $("#out_des").addClass("none");
    // $("#cl_red").removeClass("none");
}
function OpenClavaBlue(el) {
    let cal = $(el).parent().parent();
    let many = cal.children(".input_des").children("#many");
    let liter = cal.children(".input_des").children("#liter");
    many.removeClass("focus");
    liter.addClass("focus");
    many.val(null);

    liter.val(null);
    cal.children("#cl_red").addClass("none");
    cal.children("#out_des").addClass("none");
    let cl_blue = cal.children("#cl_blue");
    cl_blue.removeClass("none");
    cl_blue.get(0).oncal = false;
}
function StartDes(th) {
    let content = $(th).parent().parent();
    content.find(".clava").addClass("none");
    content.find(".out_des").removeClass("none");
    let info_ges = content.find(".info_ges");
    info_ges.text("РОБОТА");
    info_ges.css("background-color", "#dbdb18");
}

function StopDes(th) {
    let content = $(th).parent().parent();
    let info_ges = content.find(".info_ges");
    info_ges.text("ОСТАНОВ");
    info_ges.css("background-color", "green");
}