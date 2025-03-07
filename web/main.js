/**
 * Blahaj PI Coming Soon Page
 * Main JavaScript functionality
 */
(function() {
    'use strict';
    
    // Initialize when DOM is fully loaded
    document.addEventListener('DOMContentLoaded', init);
    
    /**
     * Check if the device is mobile
     * @return {boolean} True if the device is mobile
     */
    function isMobileDevice() {
        return (
            navigator.userAgent.match(/Android/i) ||
            navigator.userAgent.match(/webOS/i) ||
            navigator.userAgent.match(/iPhone/i) ||
            navigator.userAgent.match(/iPad/i) ||
            navigator.userAgent.match(/iPod/i) ||
            navigator.userAgent.match(/BlackBerry/i) ||
            navigator.userAgent.match(/Windows Phone/i)
        );
    }
    
    /**
     * Check if the device is iOS
     * @return {boolean} True if the device is iOS
     */
    function isIOS() {
        return (
            /iPad|iPhone|iPod/.test(navigator.userAgent) ||
            (navigator.platform === 'MacIntel' && navigator.maxTouchPoints > 1)
        );
    }
    
    /**
     * Initialize all page functionality
     */
    function init() {
        const elements = {
            video: document.querySelector('video'),
            videoContainer: document.getElementById('video-container'),
            videoWrapper: document.querySelector('.video-inner-wrapper'),
            logoImage: document.getElementById('logo-image'),
            contentContainer: document.querySelector('.content')
        };
        
        // Check if device is mobile but not iOS
        if (isMobileDevice() && !isIOS()) {
            // On non-iOS mobile, skip video and show logo directly
            console.log('Non-iOS mobile device detected, skipping video');
            elements.videoContainer.style.display = 'none';
            elements.logoImage.style.opacity = '1';
            
            // Add the animation-ready class to start CSS animations
            elements.contentContainer.classList.add('animation-ready');
            return;
        }
        
        // For desktop or iOS, attempt to play the video
        console.log('Desktop or iOS device detected, attempting video playback');
        setupVideoEvents(elements);
        
        // Error handling
        elements.video.addEventListener('error', (e) => {
            console.log('Video error event triggered:', e);
            elements.videoContainer.style.display = 'none';
            elements.logoImage.style.opacity = '1';
            
            // Add the animation-ready class to start CSS animations
            elements.contentContainer.classList.add('animation-ready');
        });
    }
    
    /**
     * Set up all video-related event listeners
     * @param {Object} elements - DOM elements
     */
    function setupVideoEvents(elements) {
        const { video, videoContainer } = elements;
        
        // Start transition 1 second before video ends
        video.addEventListener('timeupdate', () => {
            // If there's 0.5 second or less remaining (reduced from 1s)
            if (video.duration - video.currentTime <= 0.5 && !videoContainer.classList.contains('shrinking')) {
                startShrinkTransition(elements);
            }
        });
        
        // When video ends, show the logo
        video.addEventListener('ended', () => {
            console.log('Video ended');
            showLogo(elements);
        });
        
        // Force video to load
        video.load();
        
        // Special handling for iOS
        if (isIOS()) {
            console.log('iOS-specific video handling');
            // Try to play immediately
            playVideoWithFallback(elements);
            
            // iOS often needs user interaction, so we'll add a touch/click handler
            document.addEventListener('touchstart', function iosTouchHandler() {
                console.log('Touch detected, attempting to play video again');
                playVideoWithFallback(elements);
                // Remove the handler after first touch to avoid repeated attempts
                document.removeEventListener('touchstart', iosTouchHandler);
            }, { once: true });
        } else {
            // For desktop browsers, just play normally
            playVideoWithFallback(elements);
        }
    }
    
    /**
     * Helper function to handle video playback with proper fallback
     * @param {Object} elements - DOM elements
     */
    function playVideoWithFallback(elements) {
        const { video, videoContainer, logoImage, contentContainer } = elements;
        
        // Add a timeout to fallback if video doesn't start within 5 seconds
        const fallbackTimer = setTimeout(() => {
            console.log('Video playback timeout - falling back to static image');
            videoContainer.style.display = 'none';
            logoImage.style.opacity = '1';
            
            // Add the animation-ready class to start CSS animations
            contentContainer.classList.add('animation-ready');
        }, 5000);
        
        // Try to play the video with better error handling
        video.play().then(() => {
            console.log('Video playback started successfully');
            // Clear the fallback timer if video starts playing
            clearTimeout(fallbackTimer);
        }).catch(e => {
            console.log('Video playback error:', e);
            // Clear the fallback timer
            clearTimeout(fallbackTimer);
            // Fallback if video fails to play
            videoContainer.style.display = 'none';
            logoImage.style.opacity = '1';
            
            // Add the animation-ready class to start CSS animations
            contentContainer.classList.add('animation-ready');
        });
    }
    
    /**
     * Start the shrinking transition 0.5 second before video ends
     * @param {Object} elements - DOM elements
     */
    function startShrinkTransition(elements) {
        const { videoContainer } = elements;
        
        console.log('Starting shrink transition');
        // Add the shrinking class to start the CSS transition
        videoContainer.classList.add('shrinking');
    }
    
    /**
     * Show the logo after video ends
     * @param {Object} elements - DOM elements
     */
    function showLogo(elements) {
        const { videoContainer, logoImage, contentContainer } = elements;
        
        console.log('Showing logo');
        // Hide video container after a short delay
        setTimeout(() => {
            videoContainer.style.display = 'none';
        }, 500); // Reduced from 1000 to 500
        
        // Show logo
        logoImage.style.opacity = '1';
        
        // Start animations
        contentContainer.classList.add('animation-ready');
    }
})();