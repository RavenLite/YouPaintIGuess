/**
 * Created by Raven on 2018/10/18.
 */
var myVue = new Vue({
    el: '#app',
    data: {
        url: '127.0.0.1',
        myname: '', //我的名字
        info: [], //游戏信息
        content: '', //聊天记录
        dataurl: '',
        result: '',
        myturn: 0
    },

    mounted: function () {
        this.login();
        setInterval(this.refresh,500);
        //this.refresh();
    },

    methods: {
        // 发送登录请求
        login() {
            this.myname = prompt("欢迎来玩你画我猜，请输入你的名字(仅限英文！)", "");
            this.$http.get(this.url, {
                params: {
                    action: "login",
                    data: this.myname
                }
            }).then((response) => {
                console.log("[ACTION]login↓↓↓");
                console.log(response);
            });
        },
        refresh() {
            this.$http.get(this.url, {
                params: {
                    action: "getInfo",
                    data: this.myname
                }
            }).then((response) => {
                console.log("[ACTION]refresh↓↓↓");
                console.log(response);
                this.info = response.body;
                this.info.state = response.body.state;
                this.info.number = response.body.number;
                this.info.time = response.body.time;
                this.info.curname = response.body.curname;
                this.info.img = response.body.img;
                this.info.message = response.body.message;
                if(this.info.curname===this.myname){
                    this.myturn=1;
                }
                if(this.myturn==0 && this.info.state==="1"){
                    this.info.key = "[请作答]";
                } else {
                    this.info.key = response.body.key;
                }
            });
            this.$http.get("http://10.211.55.3:8088/api/getCanvas", {
            }).then((response) => {
                console.log("[ACTION]getImg↓↓↓");
                console.log(response);
                this.dataurl=response.bodyText;
                console.log(response.bodyText);
                var ctx = canvas.getContext('2d');
                var img = new Image();
                img.onload = function(){
                    canvas.width = canvas.parentElement.clientWidth;
                    canvas.height = img.height;
                    ctx.drawImage(img, 0, 0);
                    //cb(canvas);
                };
                img.src = this.dataurl;
            });
        },
        start() {
            this.$http.get(this.url, {
                params: {
                    action: "start",
                    data: this.myname
                }
            }).then((response) => {
                console.log("[ACTION]start↓↓↓");
                console.log(response);
            });
        },
        chat() {
            this.$http.get(this.url, {
                params: {
                    action: this.myname,
                    data: this.content
                }
            }).then((response) => {
                console.log("[ACTION]chat↓↓↓");
                console.log(response);
            });
        },
        // dataURLtoBlob(dataurl) {
        //     var arr = dataurl.split(','), mime = arr[0].match(/:(.*?);/)[1],
        //         bstr = atob(arr[1]), n = bstr.length, u8arr = new Uint8Array(n);
        //     while(n--){
        //         u8arr[n] = bstr.charCodeAt(n);
        //     }
        //     return new Blob([u8arr], {type:mime});
        // },
        // dataURItoBlob(dataURI) {
        //     // convert base64 to raw binary data held in a string
        //     // doesn't handle URLEncoded DataURIs - see SO answer #6850276 for code that does this
        //     var byteString = atob(dataURI.split(',')[1]);
          
        //     // separate out the mime component
        //     var mimeString = dataURI.split(',')[0].split(':')[1].split(';')[0]
          
        //     // write the bytes of the string to an ArrayBuffer
        //     var ab = new ArrayBuffer(byteString.length);
          
        //     // create a view into the buffer
        //     var ia = new Uint8Array(ab);
          
        //     // set the bytes of the buffer to the correct values
        //     for (var i = 0; i < byteString.length; i++) {
        //         ia[i] = byteString.charCodeAt(i);
        //     }
          
        //     // write the ArrayBuffer to a blob, and you're done
        //     var blob = new Blob([ab], {type: mimeString});
        //     return blob;
          
        // },
        uploadimg() {
            // this.dataurl = canvas.toDataURL("image/png");
            // var blob = this.dataURItoBlob(this.dataurl);
            // console.log("blob up");
            // console.log(blob);
            // console.log("blob down");

            // var reader = new FileReader();
            // reader.onload = function(e) {
            //     var myresult = reader.result;
            //     console.log("result up");
            //     console.log(myresult);
            //     console.log("result down");
            //     $.ajax({url:"127.0.0.1?action=upload&data=" + myresult,async:false});
            // }
            // reader.readAsBinaryString(blob);
            // reader.addEventListener("loadend", function() {
            //     // reader.result 包含转化为类型数组的blob
            //     this.result = reader.result;
            // });
            // reader.onload=function(f){  
            //     this.result = reader.result;
            // }
            
            // console.log("result up");
            // console.log(myresult);
            // console.log("result down");
            this.$http.post("http://10.211.55.3:8088/api/setCanvas", {
                    data: canvas.toDataURL("image/png")
                
            }).then((response) => {
                console.log("[ACTION]upload↓↓↓");
                console.log(response);
            });
        }
    }
})
// 界面元素变量
var canvas = document.getElementsByTagName('canvas')[0];
var ctx = canvas.getContext('2d'),
    msg = document.getElementById('msg'),
    ranger = document.getElementById('ranger'),
    colors = document.getElementById('colors');

var input = document.getElementById('input-msg'),
    users = document.getElementById('div-users'),
    btnIn = document.getElementById('btn-in'),
    btnAutoin = document.getElementById('btn-autoin'),
    info = document.getElementById('info'),
    tops = document.getElementById('tops');

// // 
// btnIn.inAct = function () {
//     this.innerText = '下场';
//     this.in = true;
// };
// btnIn.outAct = function () {
//     this.innerText = '上场！';
//     this.in = false;
//     this.disabled = false;
// };
// 绑定
// tops.template = tops.querySelector('[role=template]').cloneNode(true);
info.time = info.querySelector('#time');
info.player = info.querySelector('#player');
info.word = info.querySelector('#word');
// 页面载入时自动加载
window.onload = function () {
    Ctl.init();
    console.log("canvas initializes successfully.");

    //
    // btnIn.disabled = true;
    // info.player.innerText = data.name + '(自己)';
    // info.time.innerText = data.time + 's';
    // info.word.innerText = data.word;
    canvas.isMe = true;
    //

    function resize() {
        canvas.width = canvas.parentElement.clientWidth;
        canvas.paths = canvas.pts = [];
        // socket.emit('repaint');
    }
    this.addEventListener('resize', resize);
    resize();
    input.onkeydown = function (e) {
        if (e.keyCode === 13 && this.value != '') {
            // if (canvas.isMe) {
            //     alert('绘图者不能够发送消息！');
            //     return;
            // }
            //socket.emit('client msg',this.value);
            this.value = '';
        }
    }
    document.querySelector('#btns').addEventListener('click', function (e) {
        if (e.target.classList.contains('btn-active-able')) {
            if (this.prevBtn) {
                this.prevBtn.classList.remove('active')
            }
            e.target.classList.add('active')
            this.prevBtn = e.target;
        }
    }, true);

}

// 下方都是canvas相关函数，不需要关注
// 画布-鼠标移动
canvas.addEventListener('mousemove', function (e) {
    var w = 20,
        h = 20;
    if (canvas.isMe) {
        var x = e.offsetX,
            y = e.offsetY;
        if (e.buttons === 1) {
            if (!this.erase) {
                Ctl.addPos(x, y);
                Ctl.drawPts(ctx, this.pts);
                // socket.emit('paint',JSON.stringify({data:new Path(this.pts),status:'ing'}))
            } else {
                var rect = new Rect(x - (w >>> 1), y - (h >>> 1), w, h);
                rect.clearOn(ctx);
                // socket.emit('erase',rect.x,rect.y,rect.w,rect.h);
            }
        }
    }
});
// 画布-鼠标抬起
canvas.addEventListener('mouseup', function (e) {
    if (!canvas.isMe || this.erase) return;
    var x = e.offsetX,
        y = e.offsetY;
    Ctl.addPos(x, y);
    Ctl.addPath(this.pts);
    // socket.emit('paint',JSON.stringify({data:new Path(this.pts),status:'end'}));
    Ctl.clearPos();
    myVue.uploadimg();
})
// 画布-鼠标按下
canvas.addEventListener('mousedown', function (e) {
    if (!this.isMe) return;
    if (this.erase) {
        var w = 20,
            h = 20;
        var rect = new Rect(x - (w >>> 1), y - (h >>> 1), w, h);
        rect.clearOn(ctx);
        // socket.emit('erase',rect.x,rect.y,rect.w,rect.h);
        return;
    }
    var x = e.offsetX,
        y = e.offsetY;
    Ctl.clearPos();
    Ctl.addPos(x, y);
});
// 颜色选择监听器
colors.addEventListener('click', function (e) {
    var t = e.target;
    if (t.classList.contains('rect')) {
        Array.prototype.slice.call(this.getElementsByClassName('active'))
            .forEach(v => v.classList.remove('active'));
        t.classList.add('active');
        Ctl.setColor(t.style.backgroundColor);
    }
});
// 线宽选择监听器
ranger.addEventListener('change', function (e) {
    this.nextElementSibling.innerText = this.value;
    Ctl.setLw(this.value);
});
// Controller
Ctl = {
    drawPts: function (ctx, pts) {
        if (pts instanceof Path || pts.pts) {
            var color = pts.color,
                lw = pts.lw;
            pts = pts.pts;
        }
        var p1 = pts[0];
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(p1.x, p1.y);
        pts.slice(1).forEach(v => {
            ctx.lineTo(v.x, v.y);
        });
        ctx.lineWidth = lw || canvas.lw
        ctx.strokeStyle = color || canvas.color;
        ctx.stroke();
        ctx.restore();
    },
    init: function () {
        canvas.paths = [];
        canvas.pts = [];
        canvas.color = 'black';
        canvas.lw = 1;
        for (var i = 0; i < 20; i++)
            this.addColor();
    },
    setLw(lw) {
        canvas.lw = lw;
    },
    setColor(c) {
        canvas.color = c;
    },
    addPath: function (pts) {
        canvas.paths.push(new Path(pts, canvas.lw, canvas.color));
    },
    addPos: function (x, y) {
        canvas.pts.push(new Pos(x, y));
    },
    clearPos: function () {
        canvas.pts = []
    },
    addColor: function (active) {
        var rect = document.createElement('div'),
            r = this.random;
        rect.className = 'rect';
        if (active)
            rect.className += ' active';
        rect.style.backgroundColor = 'rgb(' + [r(256), r(256), r(256)].join(',') + ')';
        colors.appendChild(rect);
    },
    random: function (b) {
        return Math.floor(Math.random() * b);
    }
};

function Pos(x, y) {
    this.x = x;
    this.y = y;
}

function Path(pts, lw, color) {
    this.pts = pts;
    this.lw = lw || canvas.lw;
    this.color = color || canvas.color;
}

function Rect(x, y, w, h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
}

Rect.prototype.clearOn = function (ctx) {
    ctx.clearRect(this.x, this.y, this.w, this.h);
}