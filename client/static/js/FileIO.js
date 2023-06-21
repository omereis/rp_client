// ============================================================================
//    Author: Kenneth Perkins
//    Date:   Jul 17, 2020
//    Taken From: http://programmingnotes.org/
//    File:  FileIO.js
//    Description: Namespace which handles opening, reading and saving files
//    Example:
//        // 1. Save data to a file
//        FileIO.save({
//            data: 'My Programming Notes is awesome!',
//            filename: 'notes.txt',
//            decodeBase64Data: false, // optional
//        });
//
//        // 2. Get files
//        let files = await FileIO.openDialog({
//            contentTypes: '.txt, .json, .doc, .png', // optional
//            allowMultiple: true, // optional
//        });
//
//        // 3. Read file contents
//        let contents = await FileIO.read({
//            data: file,
//            encoding: 'UTF-8', // optional
//            readAsTypedArray: false, // optional
//        });
// ============================================================================
/**
* NAMESPACE: FileIO
* USE: Handles opening, reading and saving files.
*/
var FileIO = FileIO || {};
(function(namespace) {
    'use strict';

// -- Public data --
    // Property to hold public variables and functions
    let exposed = namespace;

    /**
    * FUNCTION: save
    * USE: Exports data to be saved to the client
    * @param options: An object of file save options.
    *   Its made up of the following properties:
    *   {
    *       data: The data (String/Blob/File/Uint8Array) to be saved.
    *       filename: Optional. The name of the file
    *       decodeBase64Data: Optional. If the data to be saved is not
    *          a Blob type, it will be converted to one. If the data
    *          being converted is a base64 string, this specifies
    *          if the string should be decoded or not when converting
    *          to a Blob.
    *   }
    * @return: N/A.
    */
    exposed.save = (options) => {
        if (isNull(options)) {
            throw new TypeError('There are no options specified');
        } else if (isNull(options.data)) {
            throw new TypeError('There is no data is specified to save');
        }

        let blob = options.data;
        let decodeBase64Data = options.decodeBase64Data || false;

        // Check to see if the file data is a blob.
        // Try to convert it if not
        if (!isBlob(blob)) {
            blob = exposed.convertToBlob(blob, decodeBase64Data);
        }

        let filename = options.filename || blob.name || 'FileIO_Download';
        let urlCreator = window.URL || window.webkitURL;
        let url = urlCreator.createObjectURL(blob);
        let a = document.createElement('a');

        a.download = filename;
        a.href = url;
        a.style.display = 'none';
        a.rel = 'noopener';
        a.target = '_blank';
        document.body.appendChild(a);
        elementClick(a);

        setTimeout(() => {
            document.body.removeChild(a);
            urlCreator.revokeObjectURL(url)
        }, 250);
    }

    /**
    * FUNCTION: openDialog
    * USE: Opens a file dialog box which enables the user to select a file
    * @param options: An object of file dialog options.
    *   Its made up of the following properties:
    *   {
    *       contentTypes: Optional. A comma separated string of
    *          acceptable content types.
    *       allowMultiple: Optional. Boolean indicating if the user
    *          can select multple files. Default is false.
    *   }
    * @return: A promise that returns the selected files when the user hits submit.
    */
    exposed.openDialog = (options) => {
        return new Promise((resolve, reject) => {
            try {
                if (isNull(options)) {
                    options = {};
                }

                // Check to see if content types is an array
                let contentTypes = options.contentTypes || '';
                if (!isNull(contentTypes) && !Array.isArray(contentTypes)) {
                    contentTypes = contentTypes.split(',');
                }

                if (!isNull(contentTypes)) {
                    contentTypes = contentTypes.join(',');
                }

                let input = document.createElement('input');
                input.type = 'file';
                input.multiple = options.allowMultiple || false;
                input.accept = contentTypes;

                input.addEventListener('change', (e) => {
                    // Convert FileList to an array
                    let files = Array.prototype.slice.call(input.files);
                    resolve(files);
                });

                input.style.display = 'none';
                document.body.appendChild(input);
                elementClick(input);

                setTimeout(() => {
                    document.body.removeChild(input);
                }, 250);
            } catch (e) {
                let message = e.message ? e.message : e;
                reject(`Failed to open dialog. Reason: ${message}`);
            }
        });
    }

    /**
    * FUNCTION: read
    * USE: Reads a file and gets its file contents
    * @param options: An object of file read options.
    *   Its made up of the following properties:
    *   {
    *       data: The data (File/Blob) to be read.
    *       encoding: Optional. String that indicates the file encoding
    *          when opening a file while reading as text. Default is UTF-8.
    *          readAsText is the default behavior.
    *       readAsTypedArray: Optional. Boolean that indicates if the file should
    *          be read as a typed array. If this option is specified, a Uint8Array
    *          object is returned on file read.
    *       onProgressChange(progressEvent): Function that is fired periodically as
    *          the FileReader reads data.
    *   }
    * @return: A promise that returns the file contents after successful file read.
    */
    exposed.read = (options) => {
        return new Promise((resolve, reject) => {
            try {
                if (!(window && window.File && window.FileList && window.FileReader)) {
                    throw new TypeError('Unable to read. Your environment does not support File API.');
                } else if (isNull(options)) {
                    // Check to see if there are options
                    throw new TypeError('There are no options specified.');
                } else if (isNull(options.data)) {
                    // Check to see if a file is specified
                    throw new TypeError('Unable to read. There is no data specified.');
                } else if (!isBlob(options.data)) {
                    // Check to see if a file is specified
                    throw new TypeError(`Unable to read data of type: ${typeof options.data}. Reason: '${options.data}' is not a Blob/File.`);
                } else if (!isNull(options.onProgressChange) && !isFunction(options.onProgressChange)) {
                    // Check to see if progress change callback is a function
                    throw new TypeError(`Unable to call onProgressChange of type: ${typeof options.onProgressChange}. Reason: '${options.onProgressChange}' is not a function.`);
                }

                let file = options.data;
                let encoding = options.encoding || 'UTF-8';
                let readAsTypedArray = options.readAsTypedArray || false;

                // Setting up the reader
                let reader = new FileReader();

                // Tell the reader what to do when it's done reading
                reader.addEventListener('load', (e) => {
                    let content = e.target.result;
                    if (readAsTypedArray) {
                        content = new Uint8Array(content)
                    }
                    resolve(content);
                });

                // Return the error
                reader.addEventListener('error', (e) => {
                    reject(reader.error);
                });

                // Call the on progress change function if specified
                if (options.onProgressChange) {
                    let events = ['loadstart', 'loadend', 'progress'];
                    events.forEach((eventName) => {
                        reader.addEventListener(eventName, (e) => {
                            options.onProgressChange.call(this, e);
                        });
                    });
                }

                // Begin reading the file
                if (readAsTypedArray) {
                    reader.readAsArrayBuffer(file);
                } else {
                    reader.readAsText(file, encoding);
                }
            } catch (e) {
                let message = e.message ? e.message : e;
                reject(`Failed to read data. Reason: ${message}`);
            }
        });
    }

    /**
    * FUNCTION: convertToBlob
    * USE: Converts data to a Blob
    * @param data: Data to be converted to a Blob.
    * @param decodeBase64Data: Indicates if the data should be base64 decoded.
    * @return: The data converted to a Blob.
    */
    exposed.convertToBlob = (data, decodeBase64Data = false) => {
        let arry = data;
        if (!isTypedArray(arry)) {
            arry = decodeBase64Data
                ? exposed.base64ToTypedArray(data)
                : exposed.stringToTypedArray(data);
        }
        let blob = new Blob([arry]);
        return blob;
    }

    /**
    * FUNCTION: stringToTypedArray
    * USE: Converts a string to a typed array (Uint8Array)
    * @param data: String to be converted to a typed array.
    * @return: The data converted to a typed array.
    */
    exposed.stringToTypedArray = (data) => {
        let arry = new Uint8Array(data.length);
        for (let index = 0; index < data.length; ++index) {
            arry[index] = data.charCodeAt(index);
        }
        return arry;
    }

    /**
    * FUNCTION: typedArrayToString
    * USE: Converts a typed array to a string
    * @param data: Typed array to be converted to a string.
    * @return: The data converted to a string.
    */
    exposed.typedArrayToString = (data) => {
        let str = '';
        for (let index = 0; index < data.length; ++index) {
            str += String.fromCharCode(data[index]);
        }
        return str;
    }

    /**
    * FUNCTION: base64ToTypedArray
    * USE: Converts a base64 string to a typed array
    * @param data: Base64 string to be converted to a typed array.
    * @return: The data converted to a typed array.
    */
    exposed.base64ToTypedArray = (data) => {
        let str = atob(data);
        let arry = exposed.stringToTypedArray(str);
        return arry;
    }

    /**
    * FUNCTION: typedArrayToBase64
    * USE: Converts a typed array to a base64 string
    * @param data: Typed array to be converted to a base64 string.
    * @return: The data converted to a base64 string.
    */
    exposed.typedArrayToBase64 = (data) => {
        let str = exposed.typedArrayToString(data);
        return btoa(str);
    }

    /**
    * FUNCTION: getFilename
    * USE: Returns the filename of a url
    * @param url: The url.
    * @return: The filename of a url.
    */
    exposed.getFilename = (url) => {
        let filename = '';
        if (url && url.length > 0) {
            filename = url.split('\\').pop().split('/').pop();
            // Remove any querystring
            if (filename.indexOf('?') > -1) {
                filename = filename.substr(0, filename.indexOf('?'));
            }
        }
        return filename;
    }

    /**
    * FUNCTION: getExtension
    * USE: Returns the file extension of the filename of a url
    * @param url: The url.
    * @return: The file extension of a url.
    */
    exposed.getExtension = (url) => {
        let filename = exposed.getFilename(url);
        let ext = filename.split('.').pop();
        return (ext === filename) ? '' : ext;
    }

// -- Private data --
    let isTypedArray = (item) => {
        return item && ArrayBuffer.isView(item) && !(item instanceof DataView);
    }

    let isBlob = (item) => {
        return item instanceof Blob;
    }

    let isNull = (item) => {
        return undefined === item || null === item
    }

    let isFunction = (item) => {
        return 'function' === typeof item
    }

    let elementClick = (element) => {
        try {
            element.dispatchEvent(new MouseEvent('click'))
        } catch (e) {
            let evt = document.createEvent('MouseEvents')
            evt.initMouseEvent('click', true, true, window, 0, 0, 0, 80,
                                20, false, false, false, false, 0, null)
            element.dispatchEvent(evt)
        }
    }

    (function (factory) {
        if (typeof define === 'function' && define.amd) {
            define([], factory);
        } else if (typeof exports === 'object') {
            module.exports = factory();
        }
    }(function() { return namespace; }));
}(FileIO)); // http://programmingnotes.org/


