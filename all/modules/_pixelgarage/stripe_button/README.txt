The stripe_button module provides a field formatter for the number (decimal) field that can display one or more
Stripe Checkout buttons with a predefined or customizable amount in one of the given currencies.

To use this Drupal module for Test and Live payments, you need to open a Stripe account (https://stripe.com/global).


See also http://stripe.com/checkout

-----------------------------------------------------------------------------
VERSION INFO
-----------------------------------------------------------------------------
Version 1.x supports the Stripe Checkout payment gateway since March 2016.


-----------------------------------------------------------------------------
ABOUT THIS MODULE
-----------------------------------------------------------------------------
This module provides a field formatter for the number decimal field. This formatter allows
to display one or more Stripe Checkout buttons in two different modes:

1) Predefined value stripe button
This formatter displays one or more buttons with a predefined fixed value and a configurable currency. Clicking this
button opens the Stripe Checkout dialog with the given amount and currency already set, ready to be paid right away.

2) Custom value stripe button
This formatter displays one button with a text field to add a custom amount in a configurable currency. Clicking this
button opens the Stripe Checkout dialog with the user specified amount in the configured currency.

Additionally a numeric field can also be formatted as Stripe fee buttons, which allows to define a percentage
of the Stripe payment button as an application fee.

1) Application fee stripe button
Add a numeric field to a content type and define one or several fee percentages. See the stripe_button API to connect
the fee button to a specific stripe button.


-----------------------------------------------------------------------------
SINGLE PAYMENT OR RECURRING PAYMENT
-----------------------------------------------------------------------------
A Stripe button can be configured to perform a single payment or a recurring payment by creating
a Stripe subscription for a user. Stripe subscriptions is an automated way to regularly charge a customer with
a certain amount.

1) Single Payments
Single payments can be easily established by defining one of the above buttons. A click on this button
opens the Stripe Checkout dialog where the payment details can be added an the payment can be performed.
Additionally a fee button can be defined to handle application fees.

2) Subscriptions
A recurring payment needs a more sophisticated setup:

- Recurring payments can only be performed by registered users. The user account is used to hold and present the
subscription to the user, so that he is able to delete its subscription at any time.

- Recurring payments should setup a webhook (https://example.com/stripe/webhook) on the Stripe server,
so that Stripe can send Charge.completed events to Drupal whenever a user with a subscription is charged
again in the Stripe payment system.


-----------------------------------------------------------------------------
CHARGE.COMPLETED WEBHOOK
-----------------------------------------------------------------------------
Charge.completed webhook callbacks from Stripe are processed by the stripe_button module. The implementation
guarantees that each event is only processed once. Only a new incoming charge.completed event calls the API hook
method hook_stripe_button_charge_completed (see stripe_button.api.php).



-----------------------------------------------------------------------------
INSTALLATION AND CONFIGURATION
-----------------------------------------------------------------------------
Stripe Library:
This module requires the Stripe PHP API library. Follow these steps to install the
Stripe library and this module and to configure the Stripe access:

1. Install and enable Libraries API module (see https://www.drupal.org/project/libraries).
2. Create a stripe folder in the libraries directory (sites/all/libraries) and download the Stripe PHP library from GitHub in it.
3. Download and install the stripe_api and the stripe_button (this module) modules.
4. Configure the Stripe account access on the configuration page of the Stripe API (/admin/config/services/stripe_api).
5. Add a decimal number field to one of your content types and configure it in the view mode.
6. Set one of the two field formatters (Predefined or Custom value button) and configure more options for the Checkout dialog
    - the currency
    - the dialog title
    - the dialog description
    - the dialog button label (the amount and currency are always added at the end of the defined label)
    - if a billing address form should be displayed
    - if a shipping address form should be displayed
    - if one click payment (remember me) should be enabled


-----------------------------------------------------------------------------
CREDITS
-----------------------------------------------------------------------------
This project uses the really great Payment Gateway of Stripe, a new era of payment, simple, secure and fast.

Sponsored by Pixelgarage (http://www.pixelgarage.ch)
