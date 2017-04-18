Transloadit PHP SDK Drupal module
---------------------------------
Maintained by: Ralph Moser, Pixelgarage (ralph@ramosoft.ch)

This module provides a simple abstraction to use the Transloadit PHP SDK.

-----------------------------------------------------------------------------
Installation of Transloadit Library
-----------------------------------------------------------------------------
The Transloadit PHP SDK is installed as drupal library in the sites/all/libraries folder.
Follow these steps to create the transloadit library:

1. Install and enable Libraries API module (see https://www.drupal.org/project/libraries).
2. Download the latest release of the Transloadit PHP SDK from https://github.com/transloadit/php-sdk/archive/master.zip.
3. Rename the unzipped folder to transloadit and copy this folder into the sites/all/libraries folder, so that the
   Transloadit.php file is at location sites/all/libraries/transloadit/lib/transloadit/Transloadit.php
4. You are done!


-----------------------------------------------------------------------------
Usage of Transloadit module
-----------------------------------------------------------------------------

The module provides a simple abstraction of the Transloadit RESTful service using the Transloadit PHP SDK.

1. The module provides a configuration form at admin/config/services/transloadit_api. Here the key and the secret of
   the user account has to be added and the connection to the Transloadit service can be tested.

2. The module provides two Transloadit API call methods (see transloadit_api.methods.inc):
  a) transloadit_api_get_instance(), returning an instance of \transloadit\Transloadit with the key and secret set.
  b) transloadit_api_execute_assembly($files, $template_id, $steps = array(), $context = array(), $async = true)
     to upload the given files and process them with the defined template or the given assembly steps.
     Signature Authentication is automatically added.

3. The module provides two hooks returning the transloadit conversion result of an asynchronous request in a response object.
    (see transloadit_api.api.php for further details)


-----------------------------------------------------------------------------
Usage of Localtunnel (test webhooks locally)
-----------------------------------------------------------------------------

The Localtunnel can be used to test webhooks locally. The tool can be installed with the Node Package Manager (npm):

npm install -g localtunnel

Assuming your local server is running on port 80, just use the lt command to start the tunnel.

lt --subdomain my-project --port 80

Thats it! It will connect to the tunnel server, setup the tunnel, and tell you what url to use for your testing.
The tunnel IP will always be the same, if you define a subdomain, e.g. http://my-project.localtunnel.me

1. Copy this url to the Localtunnel URL field in the admin config form (admin/config/services/transloadit_api)
2. All webhook callbacks will automatically use the tunnel for the assembly notification.
3. With transloadit/test/[%template_id% | default] you can start a test assembly with
 the given template id on a test movie file.
3. Close the terminal session to close the tunnel after testing (or use Ctrl+c in Terminal).

