var assert = require('assert')
var crypto = require('crypto')
var fs = require('fs')
var path = require('path')
var resemble = require('node-resemble-js')

resemble.outputSettings({
    errorType: 'movement',
    transparency: 0.4
});


var pluginPath = path.resolve(__dirname, '../');
function configure(args) {
    var options = [
        '-N', // Turn off compatibility mode
        '-u', 'NORC', // Don't load .vimrc
        '-U', 'NORC', // Don't load .gvimrc
        // Disable swap files, set the runtime path to target this repository, disable the startup message
        '--cmd', '"set noswapfile | set rtp+=' + pluginPath + '" | set shortmess+=I',
        '+"set title"',
        '+"set titlestring=wimproved.vim"'];
    return options.concat(args);
}

function launchVimAndTakeScreenshot(outputPath, options, complete) {
    var spawn = require('child_process').spawn;
    var process = spawn('gvim', options, { windowsVerbatimArguments: true });
    process.on('error', function(error) {
        complete(error)
    });

    process.on('close', function(code) {
        complete();
    });

    try {
        var exec = require('child_process').execSync;
        exec('powershell -ep Bypass ./test/Take-Vim-Screenshot.ps1 ' + outputPath);
    } finally {
        process.kill('SIGTERM');
    }
}

function imageName(dir, id) {
    return path.join(dir, id + '-result.png');
}

function imageDiffName(dir, id) {
    return path.join(dir, id + '-diff.png');
}

function takeScreenshotAndCompare(outputDir, id, referenceImage, vimSettings, done) {
    var config = configure(vimSettings);

    var filename = imageName(outputDir, id);
    function afterScreenshot(error) {
        if (error) {
            done(error)
        }

        var screenshot = fs.readFileSync(filename);
        var reference = fs.readFileSync(referenceImage);
        var api = resemble(screenshot).compareTo(reference).onComplete(function(data) {

            var stream = fs.createWriteStream(imageDiffName(outputDir, id));
            stream.on('finish', function() {
                if (Number(data.misMatchPercentage) > 0.01) {
                    done(new Error('Visual difference of ' + Number(data.misMatchPercentage) + '% detected.'));
                } else {
                    done();
                }
            });

            data.getDiffImage().pack().pipe(stream);
        });
    }

    launchVimAndTakeScreenshot(filename, config, afterScreenshot);
}

describe(':WToggleClean', function() {
    var uniqueId = crypto.randomBytes(6).toString('hex');

    var outputDir = path.join('test-output', uniqueId);
    fs.existsSync(outputDir) || fs.mkdirSync(outputDir);

    var tests = [
        {
            desc: 'should work with the default theme',
            ref: 'test/References/clean_default.png',
            args: ['+WToggleClean']
        },
        {
            desc: 'should work with a dark theme',
            ref: 'test/References/clean_dark.png',
            args: ['+"colorscheme desert"', '+WToggleClean']
        }
    ]

    tests.forEach(function(test, i) {
        it('@' + i + ' ' + test.desc, function(done) {
            takeScreenshotAndCompare(outputDir, i, test.ref, test.args, done);
        });
    });
});
