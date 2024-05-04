const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html lang="en">
<div class="divLeft">  
  <p id="X1"></p>
  <p id="Y1"></p>
  <p id="press1"></p>
  <p id="statusTouch1"></p>
</div>

<div class="divRight">  
  <p id="X2"></p>
  <p id="Y2"></p>
  <p id="press2"></p>
  <p id="statusTouch2"></p>
</div>

<div class="slider-row">
  <div class="slider LeftFront">
    <p>LeftFront: <span id="leftFrontValue">0</span></p>
    <input type="range" min="0" max="100" value="0" oninput="updateLeftFrontValue(this.value)">
  </div>
  <div class="slider RightFront">
    <p>RightFront: <span id="rightFrontValue">0</span></p>
    <input type="range" min="0" max="100" value="0" oninput="updateRightFrontValue(this.value)">
  </div>
</div>
<div class="slider-row">
  <div class="slider LeftBack">
    <p>LeftBack: <span id="leftBackValue">0</span></p>
    <input type="range" min="0" max="100" value="0" oninput="updateLeftBackValue(this.value)">
  </div>
  <div class="slider RightBack">
    <p>RightBack: <span id="rightBackValue">0</span></p>
    <input type="range" min="0" max="100" value="0" oninput="updateRightBackValue(this.value)">
  </div>
</div>
  <p id="debug"></p>
  <canvas id="joystick1" style="border:1px solid #d3d3d3;"></canvas>
  <canvas id="joystick2" style="border:1px solid #d3d3d3;"></canvas>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Joystick Control</title>
  <style>
    body {
      text-align: center;
    }
    #joystick1 {
      position: absolute;
      bottom: 10px;
      left: 10px;
    }
    #joystick2 {
      position: absolute;
      bottom: 10px;
      right: 10px;
    }
.slider-row {
  display: flex;
  justify-content: center;
  align-items: center;
}

.slider {
  width: 160px;
  height: 60px;
  //border: 5px outset red;
  margin: 0px;
}
    div.divLeft {
    width: 160px;
    height: 60px;
    position: absolute;
    border: 5px outset red;
    background-color: lightblue;
    left: 10px;
    top: 10px;
    }

    div.divRight {
    width: 160px;
    height: 60px;
    position: absolute;
    border: 5px outset red;
    background-color: lightblue;
    //text-align: right;
    right: 10px;
    top: 10px;   
    }
    p {
    line-height: 4px;
    margin-top: 10px;
    }
  </style>
  <script>
    var ws;
	  var joystick = document.getElementById('joystick1');
    var ctx = joystick.getContext("2d");
    var centerX = Math.sqrt(Math.pow(window.innerWidth/10, 2) + Math.pow(window.innerHeight/10, 2));
    var centerY = Math.sqrt(Math.pow(window.innerWidth/10, 2) + Math.pow(window.innerHeight/10, 2));
    var movinX = centerX;
    var movinY = centerY;
	  var outRadius = Math.sqrt(Math.pow(window.innerWidth/12, 2) + Math.pow(window.innerHeight/12, 2)); //window.innerWidth/12; //optimalizovat
    var inRadius = outRadius/3;
    var pressed = 0; 
    var touchId = null;

    // Add new variables for the second joystick
    var joystick2 = document.getElementById('joystick2');
    var ctx2 = joystick2.getContext("2d");
    var centerX2 = Math.sqrt(Math.pow(window.innerWidth/10, 2) + Math.pow(window.innerHeight/10, 2));
    var centerY2 = Math.sqrt(Math.pow(window.innerWidth/10, 2) + Math.pow(window.innerHeight/10, 2));
    var movinX2 = centerX2;
    var movinY2 = centerY2;
    var outRadius2 = Math.sqrt(Math.pow(window.innerWidth/12, 2) + Math.pow(window.innerHeight/12, 2)); //window.innerWidth/12; //optimalizovat
    var inRadius2 = outRadius2/3;
    var pressed2 = 0;
    var touchId2 = null;
    function init() {	
        
        joystick.width = Math.sqrt(Math.pow(window.innerWidth/5, 2) + Math.pow(window.innerHeight/5, 2));
  		joystick.height = Math.sqrt(Math.pow(window.innerWidth/5, 2) + Math.pow(window.innerHeight/5, 2));
    	joystick.style.border = '5px solid red'
		
        drawExternal()
    	drawInternal()
             // Initialize the second joystick
        joystick2.width = Math.sqrt(Math.pow(window.innerWidth/5, 2) + Math.pow(window.innerHeight/5, 2));
        joystick2.height = Math.sqrt(Math.pow(window.innerWidth/5, 2) + Math.pow(window.innerHeight/5, 2));
        joystick2.style.border = '5px solid red';

        drawExternal2();
        drawInternal2();

        document.getElementById("X1").innerHTML = (centerX-movinX)/outRadius;
        document.getElementById("Y1").innerHTML = (centerY-movinY)/outRadius;
        document.getElementById("press1").innerHTML = pressed;
        document.getElementById("statusTouch1").innerHTML = touchId;

        document.getElementById("X2").innerHTML = (centerX-movinX)/outRadius;
        document.getElementById("Y2").innerHTML = (centerY-movinY)/outRadius;
        document.getElementById("press2").innerHTML = pressed;
        document.getElementById("statusTouch2").innerHTML = touchId;

        if("ontouchstart" in document.documentElement) {
        joystick.addEventListener("touchstart", touchStart, false);
        joystick.addEventListener("touchmove", touchMove, false);
        joystick.addEventListener("touchend", touchEnd, false);
        }
    	else {
        joystick.addEventListener("mousedown", mouseDown, false);
        joystick.addEventListener("mousemove", mouseMove, false);
        joystick.addEventListener("mouseup", mouseUp, false);
        }

        if("ontouchstart" in document.documentElement) {
        joystick2.addEventListener("touchstart", touchStart2, false);
        joystick2.addEventListener("touchmove", touchMove2, false);
        joystick2.addEventListener("touchend", touchEnd2, false);
        }
    	else {
        joystick2.addEventListener("mousedown", mouseDown2, false);
        joystick2.addEventListener("mousemove", mouseMove2, false);
        joystick2.addEventListener("mouseup", mouseUp2, false);
        }

        ws = new WebSocket("ws://" + window.location.host + ":81");
  		document.getElementById("ws_state").innerHTML = "CONNECTING";

  		ws.onopen  = function(){ document.getElementById("ws_state").innerHTML = "CONNECTED" };
  		ws.onclose = function(){ document.getElementById("ws_state").innerHTML = "CLOSED"};
  		ws.onerror = function(){ alert("websocket error " + this.url) };

  		ws.onmessage = ws_onmessage;
    }
    


    function drawExternal() //function to draw outer ring 1
    {
        ctx.beginPath();
        ctx.arc(centerX, centerY, outRadius, 0, 2*Math.PI, false);
        ctx.stroke();
    }
    
    function drawInternal() //function to draw inner ring 1 
    {
    ctx.beginPath();
   	ctx.arc(movinX, movinY, inRadius, 0, 2*Math.PI, false);
    ctx.fill();
    ctx.stroke();
    }

    function drawExternal2() { //function to draw outer ring 2 
      ctx2.beginPath();
      ctx2.arc(centerX2, centerY2, outRadius2, 0, 2*Math.PI, false);
      ctx2.stroke();
    }

    function drawInternal2() { //function to draw inner ring 2 
      ctx2.beginPath();
      ctx2.arc(movinX2, movinY2, inRadius2, 0, 2*Math.PI, false);
      ctx2.fill();
      ctx2.stroke();
    }
    
    function touchStart(event) //when touchable device start touch on canvas
    {
        pressed = 1;
        touchId = event.targetTouches[0].identifier;
    
        document.getElementById("press1").innerHTML = pressed; //for control fcn on diplay
    }
    
    function touchMove(event) //during touch event on canvas
    {
        if(pressed === 1 && event.targetTouches[0].target === joystick)
        {
        event.preventDefault();
        var rect = event.target.getBoundingClientRect();
        var xTouch = event.targetTouches[0].pageX - rect.left;
        var yTouch = event.targetTouches[0].pageY - rect.top;
        var dist= Math.sqrt(Math.pow(xTouch - centerX, 2) + Math.pow(yTouch - centerY, 2));
       	  	if(dist<=outRadius) 
         	{ 
         		movinX = xTouch; 
            movinY = yTouch;                
         	}
            else
            {
            	var angle = Math.atan2(yTouch - centerY, xTouch - centerX);
				      movinX=centerX+outRadius*Math.cos(angle); 
            	movinY=centerY+outRadius*Math.sin(angle); 
            }
                
        var X = (centerX-movinX)/outRadius;
        var Y = (centerY-movinY)/outRadius;
        document.getElementById("X1").innerHTML = X;
        document.getElementById("Y1").innerHTML = Y;

        // Delete canvas
        ctx.clearRect(0, 0, joystick.width, joystick.height);
         // Redraw object
        drawExternal();
        drawInternal();
        
        document.getElementById("press1").innerHTML = pressed;

            if(ws.readyState == 1) 
            {
            ws.send("1:" + X + ":" + Y + "\r\n"); // x=roll, y= pitch
            }
        }
    }
    function touchEnd(event)
    {
        if (event.changedTouches[0].identifier !== touchId) {return}
        
        pressed = 0;        
        document.getElementById("press1").innerHTML = pressed;

        movinX = centerX;
        movinY = centerY;

        var X = (centerX-movinX)/outRadius;
        var Y = (centerY-movinY)/outRadius;

        document.getElementById("X1").innerHTML = X;
        document.getElementById("Y1").innerHTML = Y;

        ctx.clearRect(0, 0, joystick.width, joystick.height);

        drawExternal();
        drawInternal();

        if(ws.readyState == 1)
        	ws.send("1:" + X + ":" + Y + "\r\n"); // x=roll, y= pitch
    }
    function mouseDown(event) 
    {
        pressed = 1;
        document.getElementById("press1").innerHTML = pressed;
    }
    
    function mouseMove(event) 
    {
    	if(pressed === 1) 
        {  
        var dist= Math.sqrt(Math.pow(event.offsetX - centerX, 2) + Math.pow(event.offsetY - centerY, 2))
       	  	if(dist<=outRadius) 
         	{ 
         		movinX=event.offsetX; 
            	movinY=event.offsetY; 
         	}
            else
            {
            	var angle = Math.atan2(event.offsetY - centerY, event.offsetX - centerX);
				movinX=centerX+outRadius*Math.cos(angle); 
            	movinY=centerY+outRadius*Math.sin(angle); 
            }
		
        var X = (centerX-movinX)/outRadius;
        var Y = (centerY-movinY)/outRadius;
        
        document.getElementById("X1").innerHTML = X;
        document.getElementById("Y1").innerHTML = Y;
        
     	ctx.clearRect(0, 0, joystick.width, joystick.height);
		drawExternal()
    	drawInternal()

            if(ws.readyState == 1) 
            {
            ws.send("1:" + X + ":" + Y + "\r\n"); // x=roll, y= pitch
            }

        }
	 	
    }
    function mouseUp(event) 
    	{
        	pressed = 0;
            document.getElementById("press1").innerHTML = pressed;
            movinX = centerX;
        	movinY = centerY;
            var X = (centerX-movinX)/outRadius;
            var Y = (centerY-movinY)/outRadius;
            document.getElementById("X1").innerHTML = X;
        	document.getElementById("Y1").innerHTML = Y;
            
            ctx.clearRect(0, 0, joystick.width, joystick.height);
			drawExternal()
    		drawInternal()
            
            
            if(ws.readyState == 1) 
                ws.send("1:" + X + ":" + Y + "\r\n"); // x=roll, y= pitch
    	}
    //________________________________________________    
    function touchStart2(event)
    {
        pressed2 = 1;
        touchId2 = event.targetTouches[0].identifier;
    
        document.getElementById("press2").innerHTML = pressed2;
    }
    
    function touchMove2(event)
    {
        if(pressed2 === 1 && event.targetTouches[0].target === joystick2)
        {
        event.preventDefault();
        var rect = event.target.getBoundingClientRect();
        var xTouch = event.targetTouches[0].pageX - rect.left;
        var yTouch = event.targetTouches[0].pageY - rect.top;
        var dist= Math.sqrt(Math.pow(xTouch - centerX2, 2) + Math.pow(yTouch - centerY2, 2));
       	  if(dist<=outRadius2) 
         	{ 
         		movinX2 = xTouch; 
            movinY2 = yTouch;                
         	}
          else
          {
            var angle = Math.atan2(yTouch - centerY2, xTouch - centerX2);
				    movinX2=centerX2+outRadius2*Math.cos(angle); 
            movinY2=centerY2+outRadius2*Math.sin(angle); 
          }
                
        var X = (centerX2-movinX2)/outRadius2;
        var Y = (centerY2-movinY2)/outRadius2;
        document.getElementById("X2").innerHTML = X;
        document.getElementById("Y2").innerHTML = Y;

        // Delete canvas
        ctx2.clearRect(0, 0, joystick2.width, joystick2.height);
         // Redraw object
        drawExternal2();
        drawInternal2();
        
        document.getElementById("press2").innerHTML = pressed2;

            if(ws.readyState == 1) 
            {
            ws.send("2:" + X + ":" + Y + "\r\n"); // x=yaw, y= power
            }
        }
    }
    function touchEnd2(event)
    {
        if (event.changedTouches[0].identifier !== touchId2) {return}
        
        pressed2 = 0;        
        document.getElementById("press2").innerHTML = pressed2;

        movinX2 = centerX2;
        movinY2 = centerY2;

        var X = (centerX2-movinX2)/outRadius2;
        var Y = (centerY2-movinY2)/outRadius2;

        document.getElementById("X2").innerHTML = X;
        document.getElementById("Y2").innerHTML = Y;

        ctx2.clearRect(0, 0, joystick2.width, joystick2.height);

        drawExternal2();
        drawInternal2();

        if(ws.readyState == 1)
        	ws.send("2:" + X + ":" + Y + "\r\n"); // x=yaw, y= power
    }
    function mouseDown2(event) 
    {
        pressed2 = 1;
        document.getElementById("press2").innerHTML = pressed2;
    }
    
    function mouseMove2(event) 
    {
    	if(pressed2 === 1) 
        {  
        var dist= Math.sqrt(Math.pow(event.offsetX - centerX2, 2) + Math.pow(event.offsetY - centerY2, 2))
       	  	if(dist<=outRadius2) 
         	{ 
         		movinX2=event.offsetX; 
            	movinY2=event.offsetY; 
         	}
            else
            {
            	var angle = Math.atan2(event.offsetY - centerY2, event.offsetX - centerX2);
				movinX2=centerX2+outRadius2*Math.cos(angle); 
            	movinY2=centerY2+outRadius2*Math.sin(angle); 
            }
		
        var X = (centerX2-movinX2)/outRadius2;
        var Y = (centerY2-movinY2)/outRadius2;
        
        document.getElementById("X2").innerHTML = X;
        document.getElementById("Y2").innerHTML = Y;
        
     	ctx2.clearRect(0, 0, joystick2.width, joystick2.height);
		drawExternal2()
    	drawInternal2()

            if(ws.readyState == 1) 
            {
            ws.send("2:" + X + ":" + Y + "\r\n"); // x=yaw, y= power
            }

        }
	 	
    }
    function mouseUp2(event) 
    	{
        	pressed2 = 0;
            document.getElementById("press2").innerHTML = pressed2;
            movinX2 = centerX2;
        	movinY2 = centerY2;
            var X = (centerX2-movinX2)/outRadius2;
            var Y = (centerY2-movinY2)/outRadius2;
            document.getElementById("X2").innerHTML = X;
        	document.getElementById("Y2").innerHTML = Y;
            
            ctx2.clearRect(0, 0, joystick2.width, joystick2.height);
			drawExternal2()
    		drawInternal2()
            
            
            if(ws.readyState == 1) 
                ws.send("2:" + X + ":" + Y + "\r\n"); // x=yaw, y= power
    	}
    function ws_onmessage(e_msg)
	{
  		e_msg = e_msg || window.event; // MessageEvent
  		alert("msg : " + e_msg.data);
	}
    function updateLeftFrontValue(value) {
    document.getElementById("leftFrontValue").textContent = value;
    if(ws.readyState == 1) 
        ws.send("leftFront:" + value + "\r\n");
    }
    function updateRightFrontValue(value) {
    document.getElementById("rightFrontValue").textContent = value;
    if(ws.readyState == 1) 
        ws.send("rightFront:" + value + "\r\n");
    }
    function updateLeftBackValue(value) {
    document.getElementById("leftBackValue").textContent = value;
    if(ws.readyState == 1) 
        ws.send("leftBack:" + value + "\r\n");
    }
    function updateRightBackValue(value) {
    document.getElementById("rightBackValue").textContent = value;
    if(ws.readyState == 1) 
        ws.send("rightBack:" + value + "\r\n");
    }
    window.onload = init;
  </script>
</body>

</html>
)=====";
