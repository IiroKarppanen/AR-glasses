// Files
String readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
int hasContent(fs::FS &fs, const char * path);

// Network
String findNetwork();
String makeRequest(const char* host, const char* widgetType);

// Control panel webpage
const char panel_html[] PROGMEM = R"=====(
<!DOCTYPE HTML><html>

<head>
  <title>AR Control Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
  <script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
</head>

<style>

    .content {
        width: 30vw;
        margin: auto;
        border: 1px solid rgb(211, 211, 211); 
        border-radius: 15px;
        padding: 25px;
    }
    @media only screen and (max-width: 600px) {
    .content {
        width: 90vw;
    }
    }

    h1   {
        color: rgb(73, 73, 73);
        font-size: 25;
        font-weight: 300;
        text-align: center;
        padding: 20px;
        height: 10vh;
    }

</style>

<body>
    <h1>Control Panel</h1>
    <div class="content">
        <div class="add-widgets">
            <div class="row">
                <div class="col text-center p-4">
                    <a href="/cycle"><button type="button" class="btn btn-primary rounded-lg">Show Next</button></a>
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Set interval to change widgets</label>
                    <div class="dropdown">
                        <button class="btn btn-dark dropdown-toggle" type="button" id="dropdownMenuButton" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                            %IntervalButton%
                        </button>
                        <div class="dropdown-menu w-100" aria-labelledby="dropdownMenuButton">
                            <a class="dropdown-item" href="/setInterval?seconds=0">No Interval</a>
                            <a class="dropdown-item" href="/setInterval?seconds=5">5s</a>
                            <a class="dropdown-item" href="/setInterval?seconds=10">10s</a>
                            <a class="dropdown-item" href="/setInterval?seconds=30">30s</a>
                            <a class="dropdown-item" href="/setInterval?seconds=60">60s</a>
                            <a class="dropdown-item" href="/setInterval?seconds=300">300s</a>
                        </div>
                </div>
            </div>
            </div>
            <div class="row">
                <div class="col text-center p-4 border-top">
                    <label>Weather Widget</label>
                    <form action="%weatherFormType%" id="weatherWidgetForm">
                        <div class="input-group">
                        <input class="form-control rounded" placeholder="%weatherInputPlaceholder%"id="weatherWidgetInput" name="weather">
                        <button type="submit" class="btn btn-success" id="weatherWidgetButton" name="weather">%weatherButtonType%</button>
                        </div>
                    </form>
                </div>
            </div>
            <div class="row">
                <div class="col text-center p-4">
                    <label>Text Widget</label>
                        <form action="%textFormType%" id="textWidgetForm">
                            <div class="input-group">
                            <input class="form-control rounded" placeholder="%textInputPlaceholder%" name="text" maxlength="24" id="textWidgetInput">
                            <button type="submit" class="btn btn-success" id="textWidgetButton" name="text">%textButtonType%</button>
                            </div>
                        </form>           
                </div>
            </div>
            
        </div>
    </div>
</body>

<script>


if (document.getElementById("weatherWidgetButton").textContent == "-") {
    document.getElementById("weatherWidgetInput").disabled = true;
    document.getElementById("weatherWidgetButton").className = "btn btn-danger";
} 
if (document.getElementById("textWidgetButton").textContent == "-") {
    document.getElementById("textWidgetInput").disabled = true;
    document.getElementById("textWidgetButton").className = "btn btn-danger";
} 

</script>

</html>
)=====";